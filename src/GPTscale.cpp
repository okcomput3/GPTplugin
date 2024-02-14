/**
 * Original code by: Scott Moreau, Daniel Kondor
 */
#include <map>
#include <memory>
#include <fstream>  
#include <wayfire/workarea.hpp>
#include <wayfire/seat.hpp>
#include <wayfire/per-output-plugin.hpp>
#include <wayfire/output.hpp>
#include <wayfire/util/duration.hpp>
#include <wayfire/view-transform.hpp>
#include <wayfire/render-manager.hpp>
#include <wayfire/workspace-set.hpp>
#include <wayfire/signal-definitions.hpp>
#include <wayfire/plugins/vswitch.hpp>
#include <wayfire/touch/touch.hpp>
#include <wayfire/plugins/scale-signal.hpp>
#include <wayfire/plugins/wobbly/wobbly-signal.hpp>

#include <wayfire/plugins/common/move-drag-interface.hpp>
#include <wayfire/plugins/common/shared-core-data.hpp>
#include <wayfire/plugins/common/input-grab.hpp>

#include <linux/input-event-codes.h>

#include "plugins/ipc/ipc-activator.hpp"
#include "GPTscale.hpp"
#include "wayfire/core.hpp"
#include "wayfire/debug.hpp"
#include "wayfire/plugin.hpp"
#include "wayfire/plugins/common/util.hpp"
#include "wayfire/scene-input.hpp"
#include "wayfire/scene.hpp"
#include "wayfire/signal-provider.hpp"
#include "wayfire/toplevel-view.hpp"
#include "wayfire/view.hpp"
#include "GPTscale.hpp"


using namespace wf::animation;

std::string escapedText;

std::string escapeQuotes(const std::string& input) {
    std::string output;
    for (char c : input) {
        if (c == '\"') {
            output += "\\\"";
        } else {
            output += c;
        }
    }
    return output;
}


class wayfire_GPTscale : public wf::per_output_plugin_instance_t,
    public wf::keyboard_interaction_t,
    public wf::pointer_interaction_t,
    public wf::touch_interaction_t
{

    bool active, hook_set;
    wf::option_wrapper_t<bool> include_minimized{"GPTscale/include_minimized"};

    /* maximum GPTscale -- 1.0 means we will not "zoom in" on a view */
    /* true if the currently running GPTscale should include views from
     * all workspaces */
    bool all_workspaces;
 //   std::unique_ptr<wf::vswitch::control_bindings_t> workspace_bindings;
    std::unique_ptr<wf::input_grab_t> grab;

    wf::plugin_activation_data_t grab_interface{
        .name = "GPTscale",
        .capabilities = wf::CAPABILITY_MANAGE_DESKTOP | wf::CAPABILITY_GRAB_INPUT,
        .cancel = [=] () { finalize(); },
    };

  public:
    void init() override
    {
        active = hook_set = false;
        grab   = std::make_unique<wf::input_grab_t>("GPTscale", output, this, this, this);
    }



    /* Activate GPTscale, switch activator modes and deactivate */
    bool handle_toggle(bool want_all_workspaces)
    {
        this->all_workspaces = want_all_workspaces;
        if (active)
        {
        } else
        {
            return activate();
        }
    }



    /* Process key event */
    void handle_keyboard_key(wf::seat_t*, wlr_keyboard_key_event ev) override
    {
        auto view = toplevel_cast(wf::get_active_view_for_output(output));
    


        if ((ev.state != WLR_KEY_PRESSED) ||
            wf::get_core().seat->get_keyboard_modifiers())
        {
            return;
        }

        switch (ev.keycode)
        {
     /*     case KEY_UP:
            next_row--;
            break;

          case KEY_DOWN:
            next_row++;
            break;

          case KEY_LEFT:
            next_col--;
            break;

          case KEY_RIGHT:
            next_col++;
            break;
    */ 
            /*
          case KEY_ENTER:
            deactivate();
            return;
*/

          case KEY_ENTER:
        {

 escapedText = escapeQuotes(chatgpt_prompt);



            // Open the file in write mode.
            std::ofstream file("chatgpt_prompt.txt");
            if (file.is_open()) {
                // Write the content to the file.
          //      file << chatgpt_prompt;
                file << escapedText;
                // Close the file.
                file.close();

                // Print a confirmation message.
                printf("Content written to file.\n");
            } else {
                // Print an error message if unable to open the file.
                printf("Unable to open file for writing.\n");
            }

            // Deactivate the plugin
            deactivate();
            return;
        }


          case KEY_ESC:
            deactivate();
           
            return;

          default:
            return;
        }

        if (!view)
        {
            return;
        }
}


    /* Returns a list of views for all workspaces */
    std::vector<wayfire_toplevel_view> get_all_workspace_views()
    {
        return output->wset()->get_views(
            (include_minimized ? 0 : wf::WSET_EXCLUDE_MINIMIZED) | wf::WSET_MAPPED_ONLY);
    }


    /* Returns a list of views to be GPTscaled */
    std::vector<wayfire_toplevel_view> get_views()
    {
        std::vector<wayfire_toplevel_view> views;

        if (all_workspaces)
        {
            views = get_all_workspace_views();
        } 

        return views;
    }

  
    /* Compute target GPTscale layout geometry for all the view transformers
     * and start animating. Initial code borrowed from the compiz GPTscale
     * plugin algorithm */
    void layout_slots(std::vector<wayfire_toplevel_view> views)
    {
        std::vector<wayfire_toplevel_view> filtered_views;
        scale_filter_signal signal(views, filtered_views);
        output->emit(&signal);
    }


    /* Activate and start GPTscale animation */
    bool activate()
    {
        if (active)
        {
            return false;
        }

        if (!output->activate_plugin(&grab_interface))
        {
            return false;
        }


        grab->grab_input(wf::scene::layer::OVERLAY);
    

        active = true;

        layout_slots(get_views());

   




        return true;
    }

    /* Deactivate and start unGPTscale animation */
    void deactivate()
    {
        active = false;

        set_hook();

        grab->ungrab_input();
        output->deactivate_plugin(&grab_interface);


        scale_end_signal signal;
        output->emit(&signal);
    }

    /* Completely end GPTscale, including animation */
    void finalize()
    {
        if (active)
        {
            /* only emit the signal if deactivate() was not called before */
            scale_end_signal signal;
            output->emit(&signal);

        }

        active = false;

        unset_hook();
        grab->ungrab_input();
        output->deactivate_plugin(&grab_interface);

        wf::scene::update(wf::get_core().scene(),
            wf::scene::update_flag::INPUT_STATE);
    }

    /* Utility hook setter */
    void set_hook()
    {
        if (hook_set)
        {
            return;
        }

        output->render->schedule_redraw();
        hook_set = true;
    }

    /* Utility hook unsetter */
    void unset_hook()
    {
        if (!hook_set)
        {
            return;
        }

        hook_set = false;
    }

    void fini() override
    {
        finalize();
    }
};

class wayfire_GPTscale_global : public wf::plugin_interface_t,
    public wf::per_output_tracker_mixin_t<wayfire_GPTscale>
{
    wf::ipc_activator_t toggle_ws{"GPTscale/toggle"};
    wf::ipc_activator_t toggle_all{"GPTscale/toggle_all"};

  public:
    void init() override
    {
        this->init_output_tracking();
        toggle_ws.set_handler(toggle_cb);
        toggle_all.set_handler(toggle_all_cb);
    }

    void fini() override
    {
        this->fini_output_tracking();
    }

    wf::ipc_activator_t::handler_t toggle_cb = [=] (wf::output_t *output, wayfire_view)
    {
        if (this->output_instance[output]->handle_toggle(false))
        {
            output->render->schedule_redraw();
            return true;
        }

        return false;
    };

    wf::ipc_activator_t::handler_t toggle_all_cb = [=] (wf::output_t *output, wayfire_view)
    {
        if (this->output_instance[output]->handle_toggle(true))
        {
            output->render->schedule_redraw();
            return true;
        }

        return false;
    };
};

DECLARE_WAYFIRE_PLUGIN(wayfire_GPTscale_global);
