#ifdef TB_PLATFORM_LINUX

#include "Tabby/Core/Application.h"
#include "Tabby/Utils/PlatformUtils.h"
#include "Tabby/Core/Time/Time.h"
#include "tbpch.h"

#include <gtk/gtk.h>

namespace Tabby {

double Time::GetTime()
{
    using namespace std::chrono;
    system_clock::time_point currentTimePoint = system_clock::now();
    duration<double> timeSinceEpoch = currentTimePoint.time_since_epoch();

    double result = timeSinceEpoch.count();

    return result;
}

struct GTKDialogContext {
    std::string path;
    bool done = false;
    bool error = false;
};

extern "C" {

static void open_done(GObject* source, GAsyncResult* result, gpointer data)
{
    GtkFileDialog* dialog = GTK_FILE_DIALOG(source);
    GFile* file;
    GError* error = NULL;
    static_cast<GTKDialogContext*>(data)->done = true;

    file = gtk_file_dialog_open_finish(dialog, result, &error);
    if (!file) {
        TB_CORE_ERROR("{0} {1} {2}", g_quark_to_string(error->domain), error->code, error->message);
        static_cast<GTKDialogContext*>(data)->error = true;
        g_error_free(error);
    } else {
        static_cast<GTKDialogContext*>(data)->path = g_file_peek_path(file);
        TB_CORE_INFO("File: {0}", static_cast<GTKDialogContext*>(data)->path);
        g_object_unref(file);
    }
}

// static void select_done(GObject* source, GAsyncResult* result, gpointer data)
// {
//     GtkFileDialog* dialog = GTK_FILE_DIALOG(source);
//     GFile* file;
//     GError* error = NULL;
//
//     file = gtk_file_dialog_select_folder_finish(dialog, result, &error);
//     if (!file) {
//         g_print("Error: %s %d %s\n", g_quark_to_string(error->domain), error->code, error->message);
//         g_error_free(error);
//     } else {
//         g_print("%s\n", g_file_peek_path(file));
//         g_object_unref(file);
//     }
// }
//
static void save_done(GObject* source, GAsyncResult* result, gpointer data)
{
    GtkFileDialog* dialog = GTK_FILE_DIALOG(source);
    GFile* file;
    GError* error = NULL;
    static_cast<GTKDialogContext*>(data)->done = true;

    file = gtk_file_dialog_save_finish(dialog, result, &error);
    if (!file) {
        TB_CORE_ERROR("{0} {1} {2}", g_quark_to_string(error->domain), error->code, error->message);
        static_cast<GTKDialogContext*>(data)->error = true;
        g_error_free(error);
    } else {
        static_cast<GTKDialogContext*>(data)->path = g_file_peek_path(file);
        TB_CORE_INFO("File: {0}", static_cast<GTKDialogContext*>(data)->path);
        g_object_unref(file);
    }
}
//
// static void open_multiple_done(GObject* source, GAsyncResult* result, gpointer data)
// {
//
//     TB_CORE_INFO("open_done");
//     GtkFileDialog* dialog = GTK_FILE_DIALOG(source);
//     GListModel* model;
//     GError* error = NULL;
//
//     model = gtk_file_dialog_open_multiple_finish(dialog, result, &error);
//     if (!model) {
//         g_print("Error: %s %d %s\n", g_quark_to_string(error->domain), error->code, error->message);
//         g_error_free(error);
//     } else {
//         for (unsigned int i = 0; i < g_list_model_get_n_items(model); i++) {
//             GFile* file = (GFile*)g_list_model_get_item(model, i);
//             g_print("%s\n", g_file_peek_path(file));
//             g_object_unref(file);
//         }
//         g_object_unref(model);
//     }
// }
//
// static void
// select_multiple_done(GObject* source,
//     GAsyncResult* result,
//     gpointer data)
// {
//     TB_CORE_INFO("open_done");
//     GtkFileDialog* dialog = GTK_FILE_DIALOG(source);
//     GListModel* model;
//     GError* error = NULL;
//
//     model = gtk_file_dialog_select_multiple_folders_finish(dialog, result, &error);
//     if (!model) {
//         g_print("Error: %s %d %s\n", g_quark_to_string(error->domain), error->code, error->message);
//         g_error_free(error);
//     } else {
//         for (unsigned int i = 0; i < g_list_model_get_n_items(model); i++) {
//             GFile* file = (GFile*)g_list_model_get_item(model, i);
//             g_print("%s\n", g_file_peek_path(file));
//             g_object_unref(file);
//         }
//         g_object_unref(model);
//     }
// }

static int cancel_dialog(gpointer data)
{

    TB_CORE_INFO("cancel_dialog");
    GCancellable* cancellable = (GCancellable*)data;

    g_cancellable_cancel(cancellable);

    return G_SOURCE_REMOVE;
}
}

std::string FileDialogs::OpenFile(const char* filter)
{
    GTKDialogContext* dialogContext = new GTKDialogContext;

    GtkFileDialog* dialog;
    GCancellable* cancellable;
    char* title = NULL;
    gboolean modal = TRUE;
    char* initial_folder = NULL;
    char* initial_name = NULL;
    char* initial_file = NULL;
    char* accept_label = NULL;
    int timeout = -1;
    GOptionContext* context;

    context = g_option_context_new("ACTION");

    gtk_init();

    dialog = gtk_file_dialog_new();

    if (title)
        gtk_file_dialog_set_title(dialog, title);
    gtk_file_dialog_set_modal(dialog, modal);
    if (initial_folder) {
        GFile* file = g_file_new_for_commandline_arg(initial_folder);
        gtk_file_dialog_set_initial_folder(dialog, file);
        g_object_unref(file);
    }
    if (initial_name)
        gtk_file_dialog_set_initial_name(dialog, initial_name);
    if (initial_file) {
        GFile* file = g_file_new_for_commandline_arg(initial_file);
        gtk_file_dialog_set_initial_file(dialog, file);
        g_object_unref(file);
    }
    if (accept_label)
        gtk_file_dialog_set_accept_label(dialog, accept_label);

    // All files
    GtkFileFilter* filefilter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(filefilter, "*");
    gtk_file_filter_set_name(filefilter, "All Files");

    // Filter extention files
    std::string stringFilter = filter;
    if (stringFilter.at(0) != '*') {
        // stringFilter.erase(0, 1);
        stringFilter = "*" + stringFilter;
    }

    GtkFileFilter* filefilter1 = gtk_file_filter_new();
    gtk_file_filter_add_pattern(filefilter1, stringFilter.c_str());
    gtk_file_filter_set_name(filefilter1, stringFilter.c_str());

    GListStore* liststore = g_list_store_new(GTK_TYPE_FILE_FILTER);
    g_list_store_append(liststore, filefilter);
    g_list_store_append(liststore, filefilter1);

    gtk_file_dialog_set_filters(dialog, G_LIST_MODEL(liststore));

    cancellable = g_cancellable_new();

    if (timeout > 0)
        g_timeout_add_seconds(timeout, cancel_dialog, cancellable);

    gtk_file_dialog_open(dialog, NULL, cancellable, (GAsyncReadyCallback)open_done, (gpointer)dialogContext);

    while (!dialogContext->done)
        g_main_context_iteration(NULL, TRUE);

    g_object_unref(dialog);

    if (dialogContext->error)
        return "";
    else
        return dialogContext->path;
}

std::string FileDialogs::SaveFile(const char* filter)
{
    GTKDialogContext* dialogContext = new GTKDialogContext;

    GtkFileDialog* dialog;
    GCancellable* cancellable;
    char* title = NULL;
    gboolean modal = TRUE;
    char* initial_folder = NULL;
    char* initial_name = NULL;
    char* initial_file = NULL;
    char* accept_label = NULL;
    int timeout = -1;
    GOptionContext* context;

    context = g_option_context_new("ACTION");

    gtk_init();

    dialog = gtk_file_dialog_new();

    if (title)
        gtk_file_dialog_set_title(dialog, title);
    gtk_file_dialog_set_modal(dialog, modal);
    if (initial_folder) {
        GFile* file = g_file_new_for_commandline_arg(initial_folder);
        gtk_file_dialog_set_initial_folder(dialog, file);
        g_object_unref(file);
    }
    if (initial_name)
        gtk_file_dialog_set_initial_name(dialog, initial_name);
    if (initial_file) {
        GFile* file = g_file_new_for_commandline_arg(initial_file);
        gtk_file_dialog_set_initial_file(dialog, file);
        g_object_unref(file);
    }
    if (accept_label)
        gtk_file_dialog_set_accept_label(dialog, accept_label);

    // All files
    GtkFileFilter* filefilter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(filefilter, "*");
    gtk_file_filter_set_name(filefilter, "All Files");

    // Filter extention files
    std::string stringFilter = filter;
    if (stringFilter.at(0) != '*') {
        // stringFilter.erase(0, 1);
        stringFilter = "*" + stringFilter;
    }

    GtkFileFilter* filefilter1 = gtk_file_filter_new();
    gtk_file_filter_add_pattern(filefilter1, stringFilter.c_str());
    gtk_file_filter_set_name(filefilter1, stringFilter.c_str());

    GListStore* liststore = g_list_store_new(GTK_TYPE_FILE_FILTER);
    g_list_store_append(liststore, filefilter);
    g_list_store_append(liststore, filefilter1);

    gtk_file_dialog_set_filters(dialog, G_LIST_MODEL(liststore));

    cancellable = g_cancellable_new();

    if (timeout > 0)
        g_timeout_add_seconds(timeout, cancel_dialog, cancellable);

    gtk_file_dialog_save(dialog, NULL, cancellable, (GAsyncReadyCallback)save_done, (gpointer)dialogContext);

    while (!dialogContext->done)
        g_main_context_iteration(NULL, TRUE);

    g_object_unref(dialog);

    if (dialogContext->error)
        return "";
    else
        return dialogContext->path;
}

}

#endif
