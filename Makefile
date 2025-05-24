# Variáveis
CC = gcc
CFLAGS = -Wall -Ilibs
LFLAGS =
PROG = malha
BUILD_DIR = build

# Arquivos fonte
SRCS = malha.c
LIB_SRCS = libs/geo.c libs/utils.c

# Arquivos objeto
OBJS = malha.o $(BUILD_DIR)/geo.o $(BUILD_DIR)/utils.o

# Regra principal
all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

# Compila malha.o no diretório atual
malha.o: malha.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compila arquivos da pasta libs e salva objetos em build/
$(BUILD_DIR)/%.o: libs/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Limpeza leve
clean:
	@echo "Limpando sujeira ..."
	@rm -f *~ *.bak malha.o $(BUILD_DIR)/*.o

# Limpeza total
purge: clean
	@echo "Limpando tudo ..."
	@rm -f $(PROG) core a.out $(DISTDIR) $(DISTDIR).tgz

# Geração do pacote de distribuição
dist: purge
	@echo "Gerando arquivo de distribuição ($(DISTDIR).tgz) ..."
	@ln -s . $(DISTDIR)
	@tar -chvzf $(DISTDIR).tgz $(addprefix ./$(DISTDIR)/, $(DISTFILES))
	@rm -f $(DISTDIR)