# Makefile for doxygen-plugin project
# Created by Iheb Eddine BADRANI, iheb-eddine@hotmail.fr, 30/09/2015


OBJ_DIR		= build
EXE_DIR		= builds

CC		= g++

ORIG_SRC_DIR	= "Doxygen Plugin"

SRC_DIR		= DoxygenPlugin
TINYXML_SRC_DIR	= $(SRC_DIR)/TinyXml
INCLUDES	= -I$(TINYXML_SRC_DIR)
CFLAGS		= -c -g $(INCLUDES)

DIR_CHECK	= $(SRC_DIR) $(OBJ_DIR) $(EXE_DIR)


OBJ_FILES	= $(OBJ_DIR)/main.o $(OBJ_DIR)/tinystr.o $(OBJ_DIR)/tinyxml.o $(OBJ_DIR)/tinyxmlerror.o $(OBJ_DIR)/tinyxmlparser.o

all:$(EXE_DIR)/doxygen-plugin
	


#		Check directories

check_dirs: $(DIR_CHECK)
	

$(SRC_DIR):
	@ln -f -s $(ORIG_SRC_DIR) $(SRC_DIR)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)


ifneq ($(OBJ_DIR), $(EXE_DIR))

$(EXE_DIR):
	@mkdir -p $(EXE_DIR)

endif



$(EXE_DIR)/doxygen-plugin: $(DIR_CHECK) $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $@



build/main.o: $(SRC_DIR)/main.cpp
	$(CC) $(CFLAGS) $(SRC_DIR)/main.cpp -o $@


$(OBJ_DIR)/tinystr.o: $(TINYXML_SRC_DIR)/tinystr.cpp
	$(CC) $(CFLAGS) $< -o $@


$(OBJ_DIR)/tinyxml.o: $(TINYXML_SRC_DIR)/tinyxml.cpp
	$(CC) $(CFLAGS) $< -o $@


$(OBJ_DIR)/tinyxmlerror.o: $(TINYXML_SRC_DIR)/tinyxmlerror.cpp
	$(CC) $(CFLAGS) $< -o $@


$(OBJ_DIR)/tinyxmlparser.o: $(TINYXML_SRC_DIR)/tinyxmlparser.cpp
	$(CC) $(CFLAGS) $< -o $@




clean:
	rm -rf $(OBJ_DIR)/ $(EXE_DIR)/
