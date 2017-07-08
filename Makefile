BIN?=bin

PLUGIN_FLAGS:=-fno-rtti -fPIC -I"$(shell $(CC) -print-file-name=plugin)/include"

$(BIN)/plugin.so: agrafuzz-gcc-pass.cpp
	@mkdir -p $(dir $@)
	$(CXX) -o "$@" -fPIC -shared $< $(PLUGIN_FLAGS)

clean:
	rm -rf $(BIN)

