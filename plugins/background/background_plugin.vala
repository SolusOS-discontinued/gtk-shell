using Peas;

public class BackgroundPlugin : ExtensionBase, Activatable {
    
    public Object object { owned get; construct; }
    
    Gtk.Window window;
    Settings settings;
    Gtk.Image image;
    
    string picture_uri;
    
    public BackgroundPlugin () {
        GLib.Object ();
    }
    
    protected void set_wallpaper () {
        try {
            var file = File.new_for_uri (picture_uri);
            image.set_from_file (file.get_path ());
        } catch (GLib.Error e) {
            GLib.message (e.message);
        }
    }
    
    public void activate () {
        var shell = object as Consort.Shell;
        window = new Gtk.Window ();
        window.set_decorated (false);
        
        image = new Gtk.Image ();
        window.add (image);
        
        /* Hook up gsettings */
        settings = new Settings ("org.gnome.desktop.background");
        this.picture_uri = settings.get_string ("picture-uri");
        
        settings.changed["picture-uri"].connect ( () => {
            this.picture_uri = settings.get_string ("picture-uri");
            this.set_wallpaper ();
        });
        
        this.set_wallpaper ();
        
        window.realize ();
        shell.set_background_window (window);
        
        // Make it full screen
        var screen = window.get_screen ();
        window.set_size_request (screen.width(), screen.height());        
        this.window.show_all ();
    }
    
    public void deactivate () {
        /* Clean up */
        this.window.set_visible (false);
        window.destroy ();
        settings = null;
        picture_uri = null;
    }
    
    public void update_state () { }
}

public class BackgroundPluginConfig : ExtensionBase, PeasGtk.Configurable {
    
    public BackgroundPluginConfig () {
        GLib.Object ();
    }
    
    public Gtk.Widget create_configure_widget () {
        string text = "Sample configuration stuffs";
        return new Gtk.Label (text);
    }
}

[ModuleInit]
public void peas_register_types (GLib.TypeModule module) {
    var objmodule = module as Peas.ObjectModule;
    objmodule.register_extension_type (typeof (Peas.Activatable), typeof (BackgroundPlugin));
    objmodule.register_extension_type (typeof (PeasGtk.Configurable), typeof (BackgroundPluginConfig));
}
