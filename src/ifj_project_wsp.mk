.PHONY: clean All

All:
	@echo ----------Building project:[ ifj_project - Debug ]----------
	@"mingw32-make.exe"  -j 2 -f "ifj_project.mk"
clean:
	@echo ----------Cleaning project:[ ifj_project - Debug ]----------
	@"mingw32-make.exe"  -j 2 -f "ifj_project.mk" clean
