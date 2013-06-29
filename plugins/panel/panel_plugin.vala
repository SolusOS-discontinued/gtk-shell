using Peas;
using Cairo;

public class PanelPlugin : ExtensionBase, Activatable {
    
    public Object object { owned get; construct; }
    
    Gtk.Window window;
    Gtk.Label clock;
    
    public PanelPlugin () {
        GLib.Object ();
    }
    
    protected bool draw_panel (Context ctx) {
        ctx.set_source_rgba(0.0,0.0,0.0,0.6);

        // Paint the entire window transparent to start with.
        ctx.set_operator(Cairo.Operator.SOURCE);
        ctx.paint();
                
        return false;
    }
    
    protected bool update_timer () {
        var now = new DateTime.now_local ();
        var lab = "<span color=\"white\">%s</span>".printf (now.format("%H:%M:%S"));
        clock.set_markup (lab);
        
        return true;
    }
    
    public void activate () {
        var shell = object as Consort.Shell;
        window = new Gtk.Window ();
        window.set_decorated (false);
        
        window.app_paintable = true;
        window.draw.connect (draw_panel);
        
        window.realize ();
        
        // Main layout
        var hbox = new Gtk.Box (Gtk.Orientation.HORIZONTAL, 0);
        window.add (hbox);
        
        // Make a simple clock.
        clock = new Gtk.Label ("");
        clock.use_markup = true;
        hbox.pack_end (clock, false, false, 0);
        
        this.update_timer ();
        
        var screen = window.get_screen ();
        var width = screen.width ();
        var height = 32;
        
        // set_visual(screen.get_rgba_visual());
        window.set_default_size (width, height);
        window.move (0,0);    
        
        shell.set_panel_window (window);
        this.window.show_all (); 
        Timeout.add_seconds (1, update_timer);   
    }
    
    
    
    public void deactivate () {
        /* Clean up */
        this.window.set_visible (false);
        window.destroy ();
    }
    
    public void update_state () { }
}

public class PanelPluginConfig : ExtensionBase, PeasGtk.Configurable {
    
    public PanelPluginConfig () {
        GLib.Object ();
    }
    
    public Gtk.Widget create_configure_widget () {
        string text = "Sample panel configuration stuffs";
        return new Gtk.Label (text);
    }
}

[ModuleInit]
public void peas_register_types (GLib.TypeModule module) {
    var objmodule = module as Peas.ObjectModule;
    objmodule.register_extension_type (typeof (Peas.Activatable), typeof (PanelPlugin));
    objmodule.register_extension_type (typeof (PeasGtk.Configurable), typeof (PanelPluginConfig));
}
