##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=ifj_project
ConfigurationName      :=Debug
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
WorkspacePath          := "D:\Documents\School\Courses\2BIB\IFJ\Project\source\trunk"
ProjectPath            := "D:\Documents\School\Courses\2BIB\IFJ\Project\source\trunk"
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=dimitrij
Date                   :=12/13/10
CodeLitePath           :="C:\Program Files\CodeLite"
LinkerName             :=gcc
ArchiveTool            :=ar rcus
SharedObjectLinkerName :=gcc -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.o.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
CompilerName           :=gcc
C_CompilerName         :=gcc
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E 
MakeDirCommand         :=makedir
CmpOptions             := -g -std=c99 -Wall -pedantic $(Preprocessors)
LinkOptions            :=  
IncludePath            :=  "$(IncludeSwitch)." "$(IncludeSwitch)." 
RcIncludePath          :=
Libs                   :=
LibPath                := "$(LibraryPathSwitch)." 


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files\CodeLite
UNIT_TEST_PP_SRC_DIR:=C:\UnitTest++-1.3
Objects=$(IntermediateDirectory)/i_list$(ObjectSuffix) $(IntermediateDirectory)/ial$(ObjectSuffix) $(IntermediateDirectory)/interpret$(ObjectSuffix) $(IntermediateDirectory)/main$(ObjectSuffix) $(IntermediateDirectory)/parser$(ObjectSuffix) $(IntermediateDirectory)/precedence_parser$(ObjectSuffix) $(IntermediateDirectory)/scaner$(ObjectSuffix) $(IntermediateDirectory)/str$(ObjectSuffix) $(IntermediateDirectory)/table$(ObjectSuffix) 

##
## Main Build Targets 
##
all: $(OutputFile)

$(OutputFile): makeDirStep $(Objects)
	@$(MakeDirCommand) $(@D)
	$(LinkerName) $(OutputSwitch)$(OutputFile) $(Objects) $(LibPath) $(Libs) $(LinkOptions)

makeDirStep:
	@$(MakeDirCommand) "./Debug"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/i_list$(ObjectSuffix): i_list.c $(IntermediateDirectory)/i_list$(DependSuffix)
	$(C_CompilerName) $(SourceSwitch) "D:/Documents/School/Courses/2BIB/IFJ/Project/source/trunk/i_list.c" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/i_list$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_list$(DependSuffix): i_list.c
	@$(C_CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/i_list$(ObjectSuffix) -MF$(IntermediateDirectory)/i_list$(DependSuffix) -MM "D:/Documents/School/Courses/2BIB/IFJ/Project/source/trunk/i_list.c"

$(IntermediateDirectory)/i_list$(PreprocessSuffix): i_list.c
	@$(C_CompilerName) $(CmpOptions) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_list$(PreprocessSuffix) "D:/Documents/School/Courses/2BIB/IFJ/Project/source/trunk/i_list.c"

$(IntermediateDirectory)/ial$(ObjectSuffix): ial.c $(IntermediateDirectory)/ial$(DependSuffix)
	$(C_CompilerName) $(SourceSwitch) "D:/Documents/School/Courses/2BIB/IFJ/Project/source/trunk/ial.c" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/ial$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ial$(DependSuffix): ial.c
	@$(C_CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/ial$(ObjectSuffix) -MF$(IntermediateDirectory)/ial$(DependSuffix) -MM "D:/Documents/School/Courses/2BIB/IFJ/Project/source/trunk/ial.c"

$(IntermediateDirectory)/ial$(PreprocessSuffix): ial.c
	@$(C_CompilerName) $(CmpOptions) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ial$(PreprocessSuffix) "D:/Documents/School/Courses/2BIB/IFJ/Project/source/trunk/ial.c"

$(IntermediateDirectory)/interpret$(ObjectSuffix): interpret.c $(IntermediateDirectory)/interpret$(DependSuffix)
	$(C_CompilerName) $(SourceSwitch) "D:/Documents/School/Courses/2BIB/IFJ/Project/source/trunk/interpret.c" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/interpret$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/interpret$(DependSuffix): interpret.c
	@$(C_CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/interpret$(ObjectSuffix) -MF$(IntermediateDirectory)/interpret$(DependSuffix) -MM "D:/Documents/School/Courses/2BIB/IFJ/Project/source/trunk/interpret.c"

$(IntermediateDirectory)/interpret$(PreprocessSuffix): interpret.c
	@$(C_CompilerName) $(CmpOptions) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/interpret$(PreprocessSuffix) "D:/Documents/School/Courses/2BIB/IFJ/Project/source/trunk/interpret.c"

$(IntermediateDirectory)/main$(ObjectSuffix): main.c $(IntermediateDirectory)/main$(DependSuffix)
	$(C_CompilerName) $(SourceSwitch) "D:/Documents/School/Courses/2BIB/IFJ/Project/source/trunk/main.c" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/main$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main$(DependSuffix): main.c
	@$(C_CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/main$(ObjectSuffix) -MF$(IntermediateDirectory)/main$(DependSuffix) -MM "D:/Documents/School/Courses/2BIB/IFJ/Project/source/trunk/main.c"

$(IntermediateDirectory)/main$(PreprocessSuffix): main.c
	@$(C_CompilerName) $(CmpOptions) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main$(PreprocessSuffix) "D:/Documents/School/Courses/2BIB/IFJ/Project/source/trunk/main.c"

$(IntermediateDirectory)/parser$(ObjectSuffix): parser.c $(IntermediateDirectory)/parser$(DependSuffix)
	$(C_CompilerName) $(SourceSwitch) "D:/Documents/School/Courses/2BIB/IFJ/Project/source/trunk/parser.c" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/parser$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/parser$(DependSuffix): parser.c
	@$(C_CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/parser$(ObjectSuffix) -MF$(IntermediateDirectory)/parser$(DependSuffix) -MM "D:/Documents/School/Courses/2BIB/IFJ/Project/source/trunk/parser.c"

$(IntermediateDirectory)/parser$(PreprocessSuffix): parser.c
	@$(C_CompilerName) $(CmpOptions) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/parser$(PreprocessSuffix) "D:/Documents/School/Courses/2BIB/IFJ/Project/source/trunk/parser.c"

$(IntermediateDirectory)/precedence_parser$(ObjectSuffix): precedence_parser.c $(IntermediateDirectory)/precedence_parser$(DependSuffix)
	$(C_CompilerName) $(SourceSwitch) "D:/Documents/School/Courses/2BIB/IFJ/Project/source/trunk/precedence_parser.c" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/precedence_parser$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/precedence_parser$(DependSuffix): precedence_parser.c
	@$(C_CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/precedence_parser$(ObjectSuffix) -MF$(IntermediateDirectory)/precedence_parser$(DependSuffix) -MM "D:/Documents/School/Courses/2BIB/IFJ/Project/source/trunk/precedence_parser.c"

$(IntermediateDirectory)/precedence_parser$(PreprocessSuffix): precedence_parser.c
	@$(C_CompilerName) $(CmpOptions) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/precedence_parser$(PreprocessSuffix) "D:/Documents/School/Courses/2BIB/IFJ/Project/source/trunk/precedence_parser.c"

$(IntermediateDirectory)/scaner$(ObjectSuffix): scaner.c $(IntermediateDirectory)/scaner$(DependSuffix)
	$(C_CompilerName) $(SourceSwitch) "D:/Documents/School/Courses/2BIB/IFJ/Project/source/trunk/scaner.c" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/scaner$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/scaner$(DependSuffix): scaner.c
	@$(C_CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/scaner$(ObjectSuffix) -MF$(IntermediateDirectory)/scaner$(DependSuffix) -MM "D:/Documents/School/Courses/2BIB/IFJ/Project/source/trunk/scaner.c"

$(IntermediateDirectory)/scaner$(PreprocessSuffix): scaner.c
	@$(C_CompilerName) $(CmpOptions) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/scaner$(PreprocessSuffix) "D:/Documents/School/Courses/2BIB/IFJ/Project/source/trunk/scaner.c"

$(IntermediateDirectory)/str$(ObjectSuffix): str.c $(IntermediateDirectory)/str$(DependSuffix)
	$(C_CompilerName) $(SourceSwitch) "D:/Documents/School/Courses/2BIB/IFJ/Project/source/trunk/str.c" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/str$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/str$(DependSuffix): str.c
	@$(C_CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/str$(ObjectSuffix) -MF$(IntermediateDirectory)/str$(DependSuffix) -MM "D:/Documents/School/Courses/2BIB/IFJ/Project/source/trunk/str.c"

$(IntermediateDirectory)/str$(PreprocessSuffix): str.c
	@$(C_CompilerName) $(CmpOptions) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/str$(PreprocessSuffix) "D:/Documents/School/Courses/2BIB/IFJ/Project/source/trunk/str.c"

$(IntermediateDirectory)/table$(ObjectSuffix): table.c $(IntermediateDirectory)/table$(DependSuffix)
	$(C_CompilerName) $(SourceSwitch) "D:/Documents/School/Courses/2BIB/IFJ/Project/source/trunk/table.c" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/table$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/table$(DependSuffix): table.c
	@$(C_CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/table$(ObjectSuffix) -MF$(IntermediateDirectory)/table$(DependSuffix) -MM "D:/Documents/School/Courses/2BIB/IFJ/Project/source/trunk/table.c"

$(IntermediateDirectory)/table$(PreprocessSuffix): table.c
	@$(C_CompilerName) $(CmpOptions) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/table$(PreprocessSuffix) "D:/Documents/School/Courses/2BIB/IFJ/Project/source/trunk/table.c"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) $(IntermediateDirectory)/i_list$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_list$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_list$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/ial$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/ial$(DependSuffix)
	$(RM) $(IntermediateDirectory)/ial$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/interpret$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/interpret$(DependSuffix)
	$(RM) $(IntermediateDirectory)/interpret$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/main$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/main$(DependSuffix)
	$(RM) $(IntermediateDirectory)/main$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/parser$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/parser$(DependSuffix)
	$(RM) $(IntermediateDirectory)/parser$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/precedence_parser$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/precedence_parser$(DependSuffix)
	$(RM) $(IntermediateDirectory)/precedence_parser$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/scaner$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/scaner$(DependSuffix)
	$(RM) $(IntermediateDirectory)/scaner$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/str$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/str$(DependSuffix)
	$(RM) $(IntermediateDirectory)/str$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/table$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/table$(DependSuffix)
	$(RM) $(IntermediateDirectory)/table$(PreprocessSuffix)
	$(RM) $(OutputFile)
	$(RM) $(OutputFile).exe


