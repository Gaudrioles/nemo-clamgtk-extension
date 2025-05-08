# Nom de l'extension et du fichier de sortie
TARGET = nemo-clamgtk.so
SOURCE = nemo-clamgtk-extension.c
INSTALL_DIR = /usr/lib/x86_64-linux-gnu/nemo/extensions-4.0
LOCALE_DIR = /usr/share/locale

# Langues prises en charge
LOCALES = fr

# Options de compilation
CFLAGS = -fPIC -Wall -Wextra \
	$(shell pkg-config --cflags libnemo-extension)

LDFLAGS = -shared -fPIC \
	$(shell pkg-config --libs libnemo-extension)

# Fichiers de traduction générés
MO_FILES = $(foreach lang, $(LOCALES), po/$(lang).mo)

# Cibles
all: $(TARGET) $(MO_FILES)

$(TARGET): $(SOURCE)
	gcc $(CFLAGS) $(SOURCE) -o $(TARGET) $(LDFLAGS)

po/%.mo: po/%.po
	msgfmt $< -o $@

install: $(TARGET) install-extension install-locales

install-extension:
	@echo "Installing extension to $(INSTALL_DIR)"
	sudo install -m 0755 $(TARGET) $(INSTALL_DIR)

install-locales: $(MO_FILES)
	@echo "Installing locales..."
	$(foreach lang, $(LOCALES), \
		sudo mkdir -p $(LOCALE_DIR)/$(lang)/LC_MESSAGES && \
		sudo install -m 0644 po/$(lang).mo $(LOCALE_DIR)/$(lang)/LC_MESSAGES/nemo-clamgtk.mo;)

clean:
	@echo "Cleaning up..."
	rm -f $(TARGET) $(MO_FILES)

clean-locales:
	@echo "Cleaning locale files..."
	rm -f $(MO_FILES)

.PHONY: all install install-extension install-locales clean clean-locales
