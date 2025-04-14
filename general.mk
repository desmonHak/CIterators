include config.mk

generate_lib: $(TARGET).a
	ar -t $(TARGET).a


all: generate_lib
	$(MAKE) -C . -f $(MAKE_NAME) examples

TESTS = code code1  

# Regla principal que genera todos los tests
examples: generate_lib $(addprefix $(PATH_EXAMPLES)/, $(addsuffix .$(EXTENSION), $(TESTS)))
	@echo "generando tests... $^"

# Regla patrón: compila cada test a partir de su fuente .c
$(PATH_EXAMPLES)/%.$(EXTENSION): $(PATH_EXAMPLES)/%.c
	$(CC) $< $(CFLAGS_EXAMPLES) -o $@ 


$(TARGET).a: $(addsuffix .o, $(OBJECTS))
	$(ARR) $(ARR_FLAGS) $@ $^
	ranlib $@

$(TARGET)_debug.a: $(addsuffix .o, $(OBJECTS))
	$(ARR) $(ARR_FLAGS) $(TARGET).a $^
	ranlib $(TARGET).a

gen_obj: $(addsuffix .o, $(OBJECTS))
	@echo "generando archivos objeto... $^"


# Regla patrón: compila cada code objeto a partir de su fuente .c
%.o: $(PATH_SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ 

cleanobj:
	$(RM) $(RMFLAGS) *.o
	

cleanall: cleanobj
	$(RM) $(RMFLAGS) $(TARGET).a *.s *.i 
	

.SILENT: clean cleanobj cleanall
.IGNORE: cleanobj cleanall
.PHONY:  cleanobj cleanall