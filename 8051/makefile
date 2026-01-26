AS = sdas8051
LD = sdld

SRC = $(wildcard *.s)

all:
	@if [ -z "$(SRC)" ]; then \
		echo "Error: No .s files found in current directory."; \
		exit 1; \
	fi; \
	echo "Found the following source files:"; \
	PS3="Please select the file to compile (enter number): "; \
	select file in $(SRC); do \
		if [ -n "$$file" ]; then \
			filename=$${file%.*}; \
			echo "Compiling $$file ..."; \
			$(AS) -lo $$file; \
			echo "Linking $${filename}.rel ..."; \
			$(LD) -i $$filename $$filename.rel; \
			if [ $$? -eq 0 ]; then \
				echo "Success! Output: $${filename}.ihx"; \
			fi; \
			break; \
		else \
			echo "Invalid selection."; \
		fi; \
	done

clean:
	@echo "Cleaning up..."
	@rm -f *.rel *.lst *.sym *.asm *.ihx *.hex *.lk *.map *.mem *.rst
	@echo "Cleaned."

.PHONY: all clean
