/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Scott Moreau
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <signal.h>
#include <wayfire/config.h>
#include <wayfire/txn/transaction-manager.hpp>
#include <wayfire/core.hpp>
#include <wayfire/seat.hpp>
#include <wayfire/scene-input.hpp>
#include <wayfire/scene-operations.hpp>
#include <wayfire/scene.hpp>
#include <wayfire/seat.hpp>
#include <wayfire/signal-provider.hpp>
#include <wayfire/toplevel-view.hpp>
#include <wayfire/util.hpp>
#include <wayfire/view.hpp>
#include <wayfire/plugin.hpp>
#include <wayfire/output.hpp>
#include <wayfire/signal-definitions.hpp>
#include <wayfire/workspace-set.hpp>
#include <wayfire/output-layout.hpp>
#include <wayfire/per-output-plugin.hpp>
#include <wayfire/unstable/wlr-view-events.hpp>
#include <wayfire/unstable/wlr-view-keyboard-interaction.hpp>
#include <wayfire/unstable/xdg-toplevel-base.hpp>
#include <wayfire/unstable/xwl-toplevel-base.hpp>
#include <wayfire/unstable/translation-node.hpp>
#include <wayfire/view-helpers.hpp>
#include <wayfire/config.h>

class wayfire_bgview_set_pointer_interaction : public wf::pointer_interaction_t
{
  public:
    void handle_pointer_enter(wf::pointf_t position) override
    {
        wf::get_core().set_cursor("default");
    }
};

// A main node for the view. It allows or denies keyboard and pointer focus according to inhibit_output.
class wayfire_background_view_root_node : public wf::scene::translation_node_t
{
    std::weak_ptr<wf::view_interface_t> _view;
    wf::option_wrapper_t<bool> inhibit_input{"background-view/inhibit_input"};
    std::unique_ptr<wf::wlr_view_keyboard_interaction_t> wlr_kb_interaction;

  public:
    wayfire_background_view_root_node(wf::view_interface_t * _view) : translation_node_t(false)
    {
        this->_view = _view->weak_from_this();
        this->wlr_kb_interaction = std::make_unique<wf::wlr_view_keyboard_interaction_t>(_view);
    }

    std::optional<wf::scene::input_node_t> find_node_at(const wf::pointf_t& point) override
    {
        if (inhibit_input)
        {
            // Prohibit pointer/touch input, but need to set the pointer on enter => we grab the input to this
            // node in those cases, and the pointer_interaction.enter() sets the cursor to default
            return wf::scene::input_node_t{
                .node = {this},
                .local_coords = point,
            };
        }

        // Propagate event to the actual view node
        return floating_inner_node_t::find_node_at(point);
    }

    wf::keyboard_focus_node_t keyboard_refocus(wf::output_t *output) override
    {
        auto view = _view.lock();
        if (inhibit_input || !view)
        {
            // Prohibit keyboard input
            return wf::keyboard_focus_node_t{};
        }

        // Take input only if the user explicitly clicks on the view => in this case, the last focus timestamp
        // on the seat will be equal to our focus timestamp.
        if (output != view->get_output())
        {
            return wf::keyboard_focus_node_t{};
        }

        const uint64_t last_ts = wf::get_core().seat->get_last_focus_timestamp();
        const uint64_t our_ts  = keyboard_interaction().last_focus_timestamp;
        if (our_ts == last_ts)
        {
            return wf::keyboard_focus_node_t{this, wf::focus_importance::REGULAR};
        }

        return wf::keyboard_focus_node_t{};
    }

    wf::pointer_interaction_t& pointer_interaction() override
    {
        static wayfire_bgview_set_pointer_interaction itr;
        return itr;
    }

    wf::keyboard_interaction_t& keyboard_interaction() override
    {
        return *wlr_kb_interaction;
    }

    std::string stringify() const override
    {
        return "background-view node " + stringify_flags();
    }
};

// The view type which background-views use.
// It derives from wf::view_interface_t directly instead of deriving from wf::toplevel_view_interface_t.
// This makes it so that the view appears similarly to a layer-shell view in the eyes of other plugins.
class unmappable_view_t : public virtual wf::view_interface_t
{
  public:
    virtual void bg_view_unmap() = 0;

    wf::wl_listener_wrapper on_unmap;
    std::shared_ptr<wayfire_background_view_root_node> root_node;

    template<class ConcreteView, class WlrObject>
    static std::shared_ptr<ConcreteView> create(
        WlrObject *toplevel, wf::output_t *output)
    {
        auto new_view = wf::view_interface_t::create<ConcreteView>(toplevel);

        new_view->role = wf::VIEW_ROLE_DESKTOP_ENVIRONMENT;
        new_view->root_node = std::make_shared<wayfire_background_view_root_node>(new_view.get());

        // Pin the view to (0, 0) on its output: backgrounds always span the whole output
        new_view->root_node->set_offset({0, 0});
        new_view->set_surface_root_node(new_view->root_node);

        new_view->set_output(output);
        wf::scene::add_front(output->node_for_layer(wf::scene::layer::BACKGROUND), new_view->get_root_node());

        // Directly map the view, we know that the old_view comes from a pre_map signal, so it is already
        // mapped on the wlroots side
        new_view->map();
        wf::view_implementation::emit_view_map_signal({new_view.get()}, true);
        return new_view;
    }
};

// An adapter so that xdg-shell views can be used as background views
class wayfire_background_view_xdg : public wf::xdg_toplevel_view_base_t, public unmappable_view_t
{
  public:
    wayfire_background_view_xdg(wlr_xdg_toplevel *toplevel) : xdg_toplevel_view_base_t(toplevel, true)
    {}

    void bg_view_unmap() override
    {
        wf::xdg_toplevel_view_base_t::unmap();
    }
};

#if WF_HAS_XWAYLAND
// An adapter so that xwayland views can be used as background views
class wayfire_background_view_xwl : public wf::xwayland_view_base_t, public unmappable_view_t
{
    wf::option_wrapper_t<bool> inhibit_input{"background-view/inhibit_input"};
  public:
    wayfire_background_view_xwl(wlr_xwayland_surface *xw) : xwayland_view_base_t(xw)
    {
        this->kb_focus_enabled = !inhibit_input;
    }

    void map()
    {
        do_map(xw->surface, true);
    }

    void bg_view_unmap() override
    {
        do_unmap();
    }
};
#endif

class wayfire_background_view : public wf::plugin_interface_t
{
    /* The command option should be set to a client like mpv, projectM or
     * a single xscreensaver.
     */
    wf::option_wrapper_t<std::string> command{"background-view/command"};

    /* The file option is for convenience when using wcm. If file is set,
     * it will be appended to the command, wrapped in double quotes.
     */
    wf::option_wrapper_t<std::string> file{"background-view/file"};

    /* The app-id option is used to identify the application. If app-id
     * is not set or does not match the launched app-id, the plugin will
     * not be able to set the client surface as the background.
     */
    wf::option_wrapper_t<std::string> app_id{"background-view/app_id"};

    // List of all background views assigned to an output
    std::map<wf::output_t*, std::shared_ptr<unmappable_view_t>> views;

  public:
    void init() override
    {
        command.set_callback(option_changed);
        file.set_callback(option_changed);
        wf::get_core().connect(&on_view_pre_map);
        option_changed();
    }

    void close_all_views()
    {
        for (auto& [output, view] : views)
        {
            view->close();
            view->on_unmap.disconnect();
            view->bg_view_unmap();
        }

        views.clear();
    }

    wf::config::option_base_t::updated_callback_t option_changed = [=] ()
    {
        close_all_views();
        if (std::string(command).empty())
        {
            return;
        }

        for (size_t i = 0; i < wf::get_core().output_layout->get_outputs().size(); i++)
        {
            wf::get_core().run(std::string(command) + add_arg_if_not_empty(file));
        }
    };

    void set_view_for_output(wayfire_toplevel_view view, wlr_surface *surface, wf::output_t *o)
    {
        std::shared_ptr<unmappable_view_t> new_view;
        if (wlr_surface_is_xdg_surface(surface))
        {
            auto toplevel = wlr_xdg_surface_from_wlr_surface(surface)->toplevel;
            wlr_xdg_toplevel_set_size(toplevel, o->get_screen_size().width, o->get_screen_size().height);
            new_view = unmappable_view_t::create<wayfire_background_view_xdg>(toplevel, o);
            new_view->on_unmap.connect(&toplevel->base->events.unmap);
#if WF_HAS_XWAYLAND
        } else if (wlr_surface_is_xwayland_surface(surface))
        {
            auto xw = wlr_xwayland_surface_from_wlr_surface(surface);
            wlr_xwayland_surface_configure(xw, o->get_layout_geometry().x, o->get_layout_geometry().y,
                o->get_layout_geometry().width, o->get_layout_geometry().height);
            new_view = unmappable_view_t::create<wayfire_background_view_xwl>(xw, o);
            new_view->on_unmap.connect(&xw->events.unmap);
        } else
#endif
        {
            LOGE("Failed to set background view: the view is neither xdg-toplevel nor a xwayland surface!");
            return;
        }

        new_view->on_unmap.set_callback([this, o] (auto)
        {
            views[o]->bg_view_unmap();
            views.erase(o);
        });
        views[o] = new_view;
    }

    wf::signal::connection_t<wf::view_pre_map_signal> on_view_pre_map = [=] (wf::view_pre_map_signal *ev)
    {
        auto view = wf::toplevel_cast(ev->view);
        if (!view)
        {
            return;
        }

        for (auto& o : wf::get_core().output_layout->get_outputs())
        {
            if (views.count(o))
            {
                continue;
            }

            /* Try to match application identifier */
            if (std::string(app_id) == view->get_app_id())
            {
                set_view_for_output(view, ev->surface, o);
                ev->override_implementation = true;
                break;
            }
        }
    };

    std::string add_arg_if_not_empty(std::string in)
    {
        if (in.empty())
        {
            return in;
        } else
        {
            return " \"" + in + "\"";
        }
    }

    void fini() override
    {
        close_all_views();
    }
};

DECLARE_WAYFIRE_PLUGIN(wayfire_background_view);
