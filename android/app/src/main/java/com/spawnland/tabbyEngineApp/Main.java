package com.spawnland.tabbyEngineApp;

import org.libsdl.app.SDLActivity;

public class Main extends SDLActivity
{
    /**
     * This method is called by SDL before loading the native shared libraries.
     * It can be overridden to provide names of shared libraries to be loaded.
     * The default implementation returns the defaults. It never returns null.
     * An array returned by a new implementation must at least contain "SDL2".
     * Also keep in mind that the order the libraries are loaded may matter.
     *
     * @return names of shared libraries to be loaded (e.g. "SDL2", "main").
     */
<<<<<<< HEAD
=======

>>>>>>> 0be7a4d412b3627228f337d78beff91934d6db7f
    @Override
    protected String[] getLibraries() {
        return new String[]{
                "SDL2",
                "App"

        };
    }
}
