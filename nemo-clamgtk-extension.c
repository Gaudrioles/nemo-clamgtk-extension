#include <nemo/libnemo-extension/nemo-menu.h>
#include <nemo/libnemo-extension/nemo-menu-item.h>
#include <nemo/libnemo-extension/nemo-menu-provider.h>
#include <nemo/libnemo-extension/nemo-column.h>
#include <nemo/libnemo-extension/nemo-column-provider.h>
#include <nemo/libnemo-extension/nemo-desktop-preferences.h>
#include <nemo/libnemo-extension/nemo-extension-types.h>
#include <nemo/libnemo-extension/nemo-name-and-desc-provider.h>
#include <nemo/libnemo-extension/nemo-file-info.h>
#include <nemo/libnemo-extension/nemo-info-provider.h>
#include <nemo/libnemo-extension/nemo-property-page.h>
#include <nemo/libnemo-extension/nemo-property-page-provider.h>
#include <nemo/libnemo-extension/nemo-location-widget-provider.h>
#include <nemo/libnemo-extension/nemo-simple-button.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>
#include <glib-2.0/glib/gi18n.h>
#include <glib-2.0/gio/gio.h>

#define UNUSED(x) (void)(x)

// Déclare les types personnalisés pour cette extension
G_BEGIN_DECLS

// Macros pour définir le type NemoClamGtk
#define NEMO_TYPE_CLAMGTK  (nemo_clamgtk_get_type())
#define NEMO_CLAMGTK(obj)  (G_TYPE_CHECK_INSTANCE_CAST((obj), NEMO_TYPE_CLAMGTK, NemoClamGtk))
#define NEMO_IS_CLAMGTK(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), NEMO_TYPE_CLAMGTK))

// Définition de la structure pour notre extension
typedef struct _NemoClamGtk
{
    GObject parent_instance; // Hérite de GObject
} NemoClamGtk;

// Définition de la classe associée
typedef struct _NemoClamGtkClass
{
    GObjectClass parent_class; // Hérite de GObjectClass
} NemoClamGtkClass;

// Prototypes pour les fonctions associées au type NemoClamGtk
GType nemo_clamgtk_get_type(void);
void nemo_clamgtk_register_type(GTypeModule *module);

G_END_DECLS

// Pointeur vers la classe parent pour des appels hérités
static GObjectClass *parent_class;

// Fonction appelée lors de l'initialisation du module
void nemo_module_initialize(GTypeModule *module)
{
    nemo_clamgtk_register_type(module);
}

// Fonction appelée lors de la fermeture du module
void nemo_module_shutdown(void)
{
    // Optionnel : Ajoutez une logique de nettoyage si nécessaire
}

// Liste les types fournis par ce module
void nemo_module_list_types(const GType **types, int *num_types)
{
    static GType type_list[1];
    type_list[0] = NEMO_TYPE_CLAMGTK; // Ajoute notre type personnalisé
    *types = type_list;
    *num_types = 1;
}

// Fonction appelée lors du clic sur l'élément du menu
static void scan_to_callback(NemoMenuItem *item, gpointer user_data)
{
    GList *files = g_object_get_data(G_OBJECT(item), "files");
    GString *cmd = g_string_new("sh"); // Prépare la commande shell
    UNUSED(user_data);

    // Parcourt tous les fichiers sélectionnés
    for (GList *scan = files; scan; scan = scan->next) {
        NemoFileInfo *file = scan->data;
        char *uri = g_file_get_path(nemo_file_info_get_location(file)); // Obtient le chemin du fichier
        g_string_append_printf(cmd, " ClamGTK -scan %s", g_shell_quote(uri)); // Ajoute le fichier à la commande
        g_free(uri);
    }

    g_spawn_command_line_async(cmd->str, NULL); // Exécute la commande
    g_string_free(cmd, TRUE); // Libère la mémoire
}

// Fournit les éléments de menu pour les fichiers
static GList *nemo_clamgtk_get_file_items(NemoMenuProvider *provider, GtkWidget *window, GList *files)
{
    UNUSED(window);
    if (!files) return NULL; // Si aucun fichier n'est sélectionné, retourne NULL

    // Crée un nouvel élément de menu
    NemoMenuItem *item = nemo_menu_item_new(
        "ClamGtkExtension::ScanWithClamGTK", // Identifiant unique
        _("Scan with ClamGTK"), // Libellé affiché
        _("Scan the selected file with ClamGTK."), // Description
        "clamav" // Icône associée
    );

    // Connecte le signal d'activation
    g_signal_connect(item, "activate", G_CALLBACK(scan_to_callback), provider);
    g_object_set_data_full(G_OBJECT(item), "files", nemo_file_info_list_copy(files), 
                           (GDestroyNotify)nemo_file_info_list_free);

    return g_list_append(NULL, item); // Retourne la liste contenant l'élément
}

// Initialise l'interface NemoMenuProvider
static void nemo_clamgtk_menu_provider_iface_init(gpointer g_iface, gpointer iface_data)
{
    NemoMenuProviderIface *iface = (NemoMenuProviderIface *)g_iface;

    // Initialisation de l'interface
    iface->get_file_items = nemo_clamgtk_get_file_items;

    (void)iface_data; // Évite un avertissement si iface_data n'est pas utilisé
}

// Initialise la classe
static void nemo_clamgtk_class_init(gpointer klass, gpointer class_data)
{
    UNUSED(class_data);
    parent_class = g_type_class_peek_parent(klass);
}


// Initialise les instances
static void nemo_clamgtk_instance_init(GTypeInstance *instance, gpointer g_class)
{
    // Initialisation de l'instance si nécessaire
    UNUSED(g_class);
    (void)instance; // Pour éviter des avertissements si non utilisé
    (void)g_class;  // Pour éviter des avertissements si non utilisé
}

// Déclare une variable pour le type
static GType clamgtk_type = 0;

// Retourne le type de l'extension
GType nemo_clamgtk_get_type(void)
{
    return clamgtk_type;
}

// Enregistre le type auprès de GLib
void nemo_clamgtk_register_type(GTypeModule *module)
{
    static const GTypeInfo info = {
        sizeof(NemoClamGtkClass),       // Taille de la classe
        NULL,                          // base_init
        NULL,                          // base_finalize
        nemo_clamgtk_class_init,       // Fonction d'initialisation de la classe
        NULL,                          // class_finalize
        NULL,                          // class_data
        sizeof(NemoClamGtk),           // Taille de l'instance
        0,                             // Nombre de préallocations
        nemo_clamgtk_instance_init,    // Fonction d'initialisation de l'instance
        NULL                           // value_table
    };

    static const GInterfaceInfo menu_provider_iface_info = {
        (GInterfaceInitFunc)nemo_clamgtk_menu_provider_iface_init, // Initialisation de l'interface
        NULL, // interface_finalize
        NULL, // interface_data
    };

    clamgtk_type = g_type_module_register_type(module, 
                                               G_TYPE_OBJECT, 
                                               "NemoClamGtk", 
                                               &info, 
                                               0); // Enregistre le type
    g_type_module_add_interface(module, 
                                clamgtk_type, 
                                NEMO_TYPE_MENU_PROVIDER, 
                                &menu_provider_iface_info); // Ajoute l'interface au type
}
