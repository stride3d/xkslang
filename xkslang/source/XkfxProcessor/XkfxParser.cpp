//
// Copyright (C)

#include <iostream>
#include <string>

#include "../Common/xkslangUtils.h"
#include "../SpxMixer/SpxCompiler.h"
#include "../XkslParser/XkslParser.h"
#include "../SpxMixer/SpxMixer.h"
#include "XkfxParser.h"

using namespace std;
using namespace xkslang;

//===================================================================================================================================
//===================================================================================================================================
// Utils
static bool error(vector<string>& msgs, const string& msg)
{
    msgs.push_back(msg);
    return false;
}

bool XkfxParser::SeparateAdotB(const string str, string& A, string& B)
{
    size_t pdot = str.find_first_of('.');
    if (pdot == string::npos) return false;
    A = str.substr(0, pdot);
    B = str.substr(pdot + 1);
    return true;
}

bool XkfxParser::IsCommandLineInstructionComplete(const string& instruction)
{
    int countParenthesis = 0;
    int countBrackets = 0;
    int countComparaisonSigns = 0;

    const char* ptr = instruction.c_str();
    while (*ptr != 0)
    {
        char c = *ptr++;
        switch (c)
        {
        case '[': countBrackets++; break;
        case ']': countBrackets--; break;
        case '(': countParenthesis++; break;
        case ')': countParenthesis--; break;
        case '<': countComparaisonSigns++; break;
        case '>': countComparaisonSigns--; break;
        }
    }

    return (countParenthesis == 0 && countComparaisonSigns == 0 && countBrackets == 0);
}

bool XkfxParser::GetNextInstruction(const std::string& line, std::string& firstInstruction, std::string& remainingLine)
{
    unsigned int startPos = 0;
    unsigned int len = line.size();

    while (startPos < len && line[startPos] == ' ') startPos++; // skip front spaces
    if (startPos == len) return false;

    char c;
    unsigned int endPos = startPos;
    while (true)
    {
        c = line[endPos];
        if (c == ' ') break;
        if (++endPos == len) break;
    }

    if (endPos == startPos) firstInstruction = "";
    else firstInstruction = line.substr(startPos, (endPos - startPos));
    if (endPos == len) remainingLine = "";
    else remainingLine = line.substr(endPos + 1);

    return true;
}

bool XkfxParser::GetNextInstruction(const string& line, string& firstInstruction, string& remainingLine, const char stopDelimiters, bool keepTheStopDelimiterInTheRemainingString)
{
    unsigned int startPos = 0;
    unsigned int len = line.size();

    while (startPos < len && line[startPos] == ' ') startPos++; // skip front spaces
    if (startPos == len) return false;

    char c;
    unsigned int endPos = startPos;
    while (true)
    {
        c = line[endPos];

        if (c == ' ')
        {
            firstInstruction = line.substr(startPos, (endPos - startPos));
            remainingLine = line.substr(endPos);
            return true;
        }

        if (c == stopDelimiters)
        {
            firstInstruction = line.substr(startPos, (endPos - startPos));
            if (!keepTheStopDelimiterInTheRemainingString) endPos++;
            remainingLine = line.substr(endPos);
            return true;
        }

        if (++endPos == len)
        {
            firstInstruction = line;
            remainingLine = "";
            return true;
        }
    }
}

string XkfxParser::GetUnmangledName(const string& fullName)
{
    size_t pos = fullName.find_first_of('<');
    if (pos == string::npos) return fullName;
    return fullName.substr(0, pos);
}

bool XkfxParser::SplitPametersString(const string& parameterStr, vector<string>& parameters)
{
    const char* ptrStr = parameterStr.c_str();
    int len = parameterStr.size();

    char c;
    int start = 0;
    int countParenthesis = 0;
    int countBrackets = 0;
    int countComparaisonSigns = 0;
    while (true)
    {
        while (ptrStr[start] == ' ' || ptrStr[start] == ',') start++;
        if (start >= len) return true;

        int end = start;
        bool loop = true;
        while (loop)
        {
            if (end == len) {
                end--;
                break;
            }

            c = ptrStr[end++];
            switch (c)
            {
                case '[': countBrackets++; break;
                case ']': countBrackets--; break;
                case '(': countParenthesis++; break;
                case ')': countParenthesis--; break;
                case '<': countComparaisonSigns++; break;
                case '>': countComparaisonSigns--; break;
                case ',':
                {
                    if (countParenthesis == 0 && countComparaisonSigns == 0 && countBrackets == 0) {
                        loop = false;
                        end--;
                    }
                    break;
                }
            }
        }

        int lastChar = end;
        while (ptrStr[lastChar] == ' ' || ptrStr[lastChar] == ',') lastChar--;
        
        parameters.push_back(parameterStr.substr(start, (lastChar - start) + 1));

        start = end + 1;
        if (start >= len) return true;
    }
}

//===================================================================================================================================
//===================================================================================================================================
// Xkfx parsing management
SpxBytecode* XkfxParser::GetSpxBytecodeForShader(const string& shaderName, string& shaderFullName,
    unordered_map<string, SpxBytecode*>& mapShaderNameBytecode, bool canLookIfUnmangledNameMatch, vector<string>& errorMsgs)
{
    auto it = mapShaderNameBytecode.find(shaderName);
    if (it != mapShaderNameBytecode.end())
    {
        SpxBytecode* spxBytecode = it->second;
        shaderFullName = it->first;
        return spxBytecode;
    }

    if (canLookIfUnmangledNameMatch)
    {
        SpxBytecode* spxBytecode = nullptr;
        for (auto it = mapShaderNameBytecode.begin(); it != mapShaderNameBytecode.end(); it++)
        {
            string anUnmangledShaderName = GetUnmangledName(it->first);
            if (anUnmangledShaderName == shaderName)
            {
                if (spxBytecode == nullptr)
                {
                    spxBytecode = it->second;
                    shaderFullName = it->first;
                }
                else
                {
                    error(errorMsgs, "2 or more shaders match the unmangled shader name: " + anUnmangledShaderName);
                    return nullptr;
                }
            }
        }
        return spxBytecode;
    }

    return nullptr;
}

class EffectMixerObject
{
public:
    string name;
    SpxMixer* mixer;

    EffectMixerObject(string name, SpxMixer* mixer) : name(name), mixer(mixer) {}
    ~EffectMixerObject() { if (mixer != nullptr) delete mixer; }
};

//===================================================================================================================================
//===================================================================================================================================
// Parsing
static bool RecordSPXShaderBytecode(string shaderFullName, SpxBytecode* spxBytecode, unordered_map<string, SpxBytecode*>& mapShaderNameBytecode, vector<string>& errorMsgs)
{
    mapShaderNameBytecode[shaderFullName] = spxBytecode;
    return true;
}

static bool RecursivelyParseAndConvertXkslShader(XkslParser* parser, const string& shaderName, glslang::CallbackRequestDataForShader callbackRequestDataForShader,
    const vector<ShaderGenericValues>& listGenericsValue, const vector<XkslUserDefinedMacro>& listUserDefinedMacros, SpxBytecode& spirXBytecode, string& infoMsg)
{
    ostringstream errorAndDebugMessages;
    bool success = parser->ConvertShaderToSpx(shaderName, callbackRequestDataForShader, listGenericsValue, listUserDefinedMacros, spirXBytecode, &errorAndDebugMessages);
    infoMsg = errorAndDebugMessages.str();

    return success;
}

static bool ConvertAndLoadRecursif(const string& stringShaderAndgenericsValue, const vector<XkslUserDefinedMacro>& listUserDefinedMacros, glslang::CallbackRequestDataForShader callbackRequestDataForShader,
    unordered_map<string, SpxBytecode*>& mapShaderNameBytecode, vector<SpxBytecode*>& listAllocatedBytecodes, XkslParser* parser, vector<string>& errorMsgs)
{
    //================================================
    //Parse and get the list of shader defintion
    vector<ShaderParsingDefinition> listshaderDefinition;
    if (!XkslParser::ParseStringWithShaderDefinitions(stringShaderAndgenericsValue.c_str(), listshaderDefinition))
        return error(errorMsgs, "Failed to parse the shaders definition from: " + stringShaderAndgenericsValue);
    vector<ShaderGenericValues> listShaderAndGenerics;
    for (unsigned int is = 0; is < listshaderDefinition.size(); is++) {
        listShaderAndGenerics.push_back(ShaderGenericValues(listshaderDefinition[is].shaderName, listshaderDefinition[is].genericsValue));
    }
    if (listShaderAndGenerics.size() == 0) return error(errorMsgs, "No shader name found");
    string shaderName = listShaderAndGenerics[0].shaderName;

    //================================================
    //Parse and convert the shader and its dependencies
    SpxBytecode* spxBytecode = nullptr;
    vector<string> vecShadersParsed;
    
    string infoMsg;
    spxBytecode = new SpxBytecode;
    listAllocatedBytecodes.push_back(spxBytecode);
    string spxOutputFileName;

    bool success = RecursivelyParseAndConvertXkslShader(parser, shaderName, callbackRequestDataForShader, listShaderAndGenerics, listUserDefinedMacros, *spxBytecode, infoMsg);

    if (!success) {
        error(errorMsgs, infoMsg);
        return error(errorMsgs, "Failed to parse and convert the XKSL file name: " + shaderName);
    }

    //Query the list of shaders from the bytecode
    if (!SpxMixer::GetListAllShadersFromBytecode(*spxBytecode, vecShadersParsed, errorMsgs))
    {
        error(errorMsgs, "Failed to get the list of shader names for: " + shaderName);
        return false;
    }

    //Store the new shaders bytecode in our map
    for (unsigned int is = 0; is < vecShadersParsed.size(); ++is)
    {
        string shaderName = vecShadersParsed[is];
        if (!RecordSPXShaderBytecode(shaderName, spxBytecode, mapShaderNameBytecode, errorMsgs))
        {
            return error(errorMsgs, "Can't add the shader into the bytecode: " + shaderName);
            return false;
        }
    }

    return true;
}

//===================================================================================================================================
//===================================================================================================================================
// Compilation
static bool CompileMixer(SpxMixer* mixer, vector<string>& errorMsgs)
{
    bool success = true;

    //TOTO: get the output stage here
    vector<OutputStageBytecode> outputStages;

    SpvBytecode compiledBytecode;
    success = mixer->Compile(outputStages, errorMsgs, nullptr, nullptr, nullptr, nullptr, &compiledBytecode, nullptr);

    if (!success) return error(errorMsgs, "Compilation Failed");

    /*{
        //write the final SPIRV bytecode then convert it into GLSL
        string outputFileNameFinalSpv = effectName + "_compile4_final.spv";
        string outputFileNameFinalSpvHr = effectName + "_compile4_final.hr.spv";
        WriteBytecode(compiledBytecode.getBytecodeStream(), outputDir, outputFileNameFinalSpv, BytecodeFileFormat::Binary);
        WriteBytecode(compiledBytecode.getBytecodeStream(), outputDir, outputFileNameFinalSpvHr, BytecodeFileFormat::Text);
    }*/

    return success;
}

//===================================================================================================================================
//===================================================================================================================================
// Compositions

//function prototype
static bool MixinShaders(const string& mixinShadersInstructionString, EffectMixerObject* mixerTarget, glslang::CallbackRequestDataForShader callbackRequestDataForShader,
    unordered_map<string, SpxBytecode*>& mapShaderNameBytecode, unordered_map<string, EffectMixerObject*>& mixerMap,
    vector<SpxBytecode*>& listAllocatedBytecodes, const vector<XkslUserDefinedMacro>& listUserDefinedMacros, XkslParser* parser, vector<string>& errorMsgs);
static EffectMixerObject* CreateAndAddNewMixer(unordered_map<string, EffectMixerObject*>& mixerMap, string newMixerName, vector<string>& errorMsgs);

static bool AddCompositionToMixer(EffectMixerObject* mixerTarget, const string& compositionString, const string& targetedShaderName, glslang::CallbackRequestDataForShader callbackRequestDataForShader,
    unordered_map<string, SpxBytecode*>& mapShaderNameBytecode, unordered_map<string, EffectMixerObject*>& mixerMap,
    vector<SpxBytecode*>& listAllocatedBytecodes, const vector<XkslUserDefinedMacro>& listUserDefinedMacros, XkslParser* parser, vector<string>& errorMsgs)
{
    bool success = false;

    //===================================================
    //composition variable target
    string compositionStr = compositionString;
    string compositionVariableTargetStr;
    string remainingStr;

    if (!XkfxParser::GetNextInstruction(compositionStr, compositionVariableTargetStr, remainingStr))
        return error(errorMsgs, "AddCompositionToMixer: Failed to find the composition variable target from: " + compositionStr);
    compositionVariableTargetStr = XkslangUtils::trim(compositionVariableTargetStr);

    //We can either have shader.variableName, or only a variableName
    string shaderName, variableName;
    if (!XkfxParser::SeparateAdotB(compositionVariableTargetStr, shaderName, variableName))
    {
        variableName = compositionVariableTargetStr;
        shaderName = targetedShaderName;
    }
    else
    {
        //if (targetedShaderName.size() > 0 && targetedShaderName.compare(shaderName) != 0)
        //    return error(errorMsgs, "The shader name (" + shaderName + ") does not match the targeted shader name (" + targetedShaderName + ")");
        //Error check's obsolete: shaderName could be a parent class of targetedShaderName
    }

    //===================================================
    //Expecting '='
    string tmpStr;
    if (!XkfxParser::GetNextInstruction(remainingStr, tmpStr, remainingStr, '=', false)) return error(errorMsgs, "\"=\" expected");
    if (XkslangUtils::trim(tmpStr).size() > 0) return error(errorMsgs, "Invalid assignation format");

    //===================================================
    //Find or create the composition source mixer
    //We can either have a mixer name, or a mixin instruction
    string mixerCompositionInstructionsStr = XkslangUtils::trim(compositionStr);

    vector<string> listCompositionInstructions;
    if (XkslangUtils::startWith(mixerCompositionInstructionsStr, "["))
    {
        if (!XkslangUtils::endWith(mixerCompositionInstructionsStr, "]"))
            return error(errorMsgs, "Invalid compositions instruction string: " + mixerCompositionInstructionsStr);

        mixerCompositionInstructionsStr = mixerCompositionInstructionsStr.substr(1, mixerCompositionInstructionsStr.size() - 2);

        //If the instruction start with '[', there are several compositions assigned to the target
        if (!XkfxParser::SplitPametersString(mixerCompositionInstructionsStr, listCompositionInstructions))
            return error(errorMsgs, "Failed to split the compositions instruction string: " + mixerCompositionInstructionsStr);
    }
    else listCompositionInstructions.push_back(mixerCompositionInstructionsStr);

    for (unsigned int iComp = 0; iComp < listCompositionInstructions.size(); iComp++)
    {
        const string compositionInstruction = listCompositionInstructions[iComp];

        EffectMixerObject* compositionSourceMixer = nullptr;
        if (XkslangUtils::startWith(compositionInstruction, "mixin(") || XkslangUtils::startWith(compositionInstruction, "mixin ")) //we accept both expressions (the 2nd is compatible with Xenko)
        {
            string mixinInstructionStr;
            if (XkslangUtils::startWith(compositionInstruction, "mixin "))
            {
                //convert mixin XXX to mixin(XXX) (to make it consistent)
                mixinInstructionStr = compositionInstruction + ")";
                mixinInstructionStr[ strlen("mixin") ] = '(';
            }
            else mixinInstructionStr = compositionInstruction;

            string mixinWord;
            XkfxParser::GetNextInstruction(mixinInstructionStr, mixinWord, mixinInstructionStr, '(', true);

            //We create a new, anonymous mixer and directly mix the shader specified in the function parameter
            string anonymousMixerInstruction;
            if (!XkslangUtils::getFunctionParameterString(mixinInstructionStr, anonymousMixerInstruction)) {
                return error(errorMsgs, "addComposition: Failed to get the instuction parameter from: \"" + mixinInstructionStr + "\"");
            }

            //Create the anonymous mixer
            string anonymousMixerName = "_anonMixer_" + to_string(mixerMap.size());
            EffectMixerObject* anonymousMixer = CreateAndAddNewMixer(mixerMap, anonymousMixerName, errorMsgs);
            if (anonymousMixer == nullptr) return error(errorMsgs, "addComposition: Failed to create a new mixer object");

            //Mix the new mixer with the shaders specified in the function parameter
            success = MixinShaders(anonymousMixerInstruction, anonymousMixer, callbackRequestDataForShader, mapShaderNameBytecode, mixerMap, listAllocatedBytecodes, listUserDefinedMacros, parser, errorMsgs);
            if (!success) return error(errorMsgs, "Mixin failed: " + mixinInstructionStr);

            compositionSourceMixer = anonymousMixer;
        }
        else
        {
            const string mixerName = compositionInstruction;

            //find the mixer in our mixer map
            if (mixerMap.find(mixerName) == mixerMap.end()) {
                return error(errorMsgs, "addComposition: no mixer found with the name: \"" + mixerName + "\"");
            }
            compositionSourceMixer = mixerMap[mixerName];
        }

        if (compositionSourceMixer == nullptr) {
            return error(errorMsgs, "addComposition: no mixer source to make the composition");
        }

        //=====================================
        // Add the composition to the mixer
        success = mixerTarget->mixer->AddCompositionInstance(shaderName, variableName, compositionSourceMixer->mixer, errorMsgs);
        if (!success) return error(errorMsgs, "Failed to add the composition to the mixer");
    }

    return true;
}

static bool AddCompositionsToMixer(EffectMixerObject* mixerTarget, const string& compositionsString, const string& targetedShaderName, glslang::CallbackRequestDataForShader callbackRequestDataForShader,
    unordered_map<string, SpxBytecode*>& mapShaderNameBytecode, unordered_map<string, EffectMixerObject*>& mixerMap,
    vector<SpxBytecode*>& listAllocatedBytecodes, const vector<XkslUserDefinedMacro>& listUserDefinedMacros, XkslParser* parser, vector<string>& errorMsgs)
{
    //split the string
    vector<string> compositions;
    if (!XkfxParser::SplitPametersString(compositionsString, compositions))
        return error(errorMsgs, "failed to split the parameters");

    if (compositions.size() == 0)
        return error(errorMsgs, "No composition found in the string: " + compositionsString);

    for (unsigned int k = 0; k < compositions.size(); ++k)
    {
        const string& compositionStr = compositions[k];
        bool success = AddCompositionToMixer(mixerTarget, compositionStr, targetedShaderName, callbackRequestDataForShader, mapShaderNameBytecode, mixerMap,
            listAllocatedBytecodes, listUserDefinedMacros, parser, errorMsgs);

        if (!success) return error(errorMsgs, "Failed to add the composition into the mixer: " + compositionStr);
    }

    return true;
}

//===================================================================================================================================
//===================================================================================================================================
// Mixin

static EffectMixerObject* CreateAndAddNewMixer(unordered_map<string, EffectMixerObject*>& mixerMap, string newMixerName, vector<string>& errorMsgs)
{
    if (mixerMap.find(newMixerName) != mixerMap.end()) {
        error(errorMsgs, "CreateAndAddNewMixer: a mixer already exists with the name:" + newMixerName);
        return nullptr;
    }

    EffectMixerObject* mixerObject = nullptr;
    SpxMixer* mixer = new SpxMixer();
    mixerObject = new EffectMixerObject(newMixerName, mixer);

    mixerMap[newMixerName] = mixerObject;
    return mixerObject;
}

static bool MixinShaders(const string& mixinShadersInstructionString, EffectMixerObject* mixerTarget, glslang::CallbackRequestDataForShader callbackRequestDataForShader,
    unordered_map<string, SpxBytecode*>& mapShaderNameBytecode, unordered_map<string, EffectMixerObject*>& mixerMap,
    vector<SpxBytecode*>& listAllocatedBytecodes, const vector<XkslUserDefinedMacro>& listUserDefinedMacros, XkslParser* parser, vector<string>& errorMsgs)
{
    if (mixinShadersInstructionString.size() == 0) return error(errorMsgs, "No shader to mix");
    bool success = true;

    //================================================
    //Parse and get the list of shader defintion
    vector<ShaderParsingDefinition> listShaderDefinition;
    if (!XkslParser::ParseStringWithShaderDefinitions(mixinShadersInstructionString.c_str(), listShaderDefinition))
        return error(errorMsgs, "mixin: failed to parse the shaders definition from: " + mixinShadersInstructionString);
    if (listShaderDefinition.size() == 0) return error(errorMsgs, "mixin: list of shader is empty");
    string shaderName = listShaderDefinition[0].shaderName;
    
    //================================================
    //Get the bytecode file and the fullName of all shader to mix into the mixer
    vector<pair<string, SpxBytecode*>> listShaderBytecodeToMix; //list of shaders to mix, and their corresponding bytecode
    {
        for (auto its = listShaderDefinition.begin(); its != listShaderDefinition.end(); its++)
        {
            const ShaderParsingDefinition& shaderDef = *its;
            string shaderName = shaderDef.GetShaderNameWithGenerics();
            string shaderFullName;   //we can omit to specify the generics when mixin a shader, we will search the best match

            SpxBytecode* shaderBytecode = XkfxParser::GetSpxBytecodeForShader(shaderName, shaderFullName, mapShaderNameBytecode, true, errorMsgs);
            if (shaderBytecode == nullptr)
            {
                //the shader bytecode does not exist, we can try its xksl file to parse and generate it
                success = ConvertAndLoadRecursif(shaderName, listUserDefinedMacros, callbackRequestDataForShader, mapShaderNameBytecode, listAllocatedBytecodes, parser, errorMsgs);
                if (!success) return error(errorMsgs, "Failed to recursively convert and load the shaders: " + shaderName);

                shaderBytecode = XkfxParser::GetSpxBytecodeForShader(shaderName, shaderFullName, mapShaderNameBytecode, true, errorMsgs);

                if (shaderBytecode == nullptr) {
                    return error(errorMsgs, "cannot find or generate a bytecode for the shader: " + shaderName);
                }
            }

            listShaderBytecodeToMix.push_back(pair<string, SpxBytecode*>(shaderFullName, shaderBytecode));
        }
    }

    unsigned int countShadersToMix = listShaderBytecodeToMix.size();
    if (countShadersToMix == 0) return error(errorMsgs, "No bytecode to mix");
    if (countShadersToMix != listShaderDefinition.size()) return error(errorMsgs, "Invalid size");

    //Previous version could mix several shaders at once into the mixer (provided that all shaders are converted into the same bytecode)
    //We now mix one shader after the previous one (to fix the same bytecode issue)
    //(also mixin shaders all-together won't give the same result at mixin one after another one)
    for (unsigned int cs = 0; cs < countShadersToMix; cs++)
    {
        const ShaderParsingDefinition& shaderDefinition = listShaderDefinition[cs];
        string shaderFullName = listShaderBytecodeToMix[cs].first;
        SpxBytecode* shaderBytecode = listShaderBytecodeToMix[cs].second;
        vector<string> listShaderToMix = { shaderFullName }; //mixer can accept a list of shaders to mix (but we disable it for now)

        //=====================================================
        //Mixin the bytecode into the mixer
        {
            success = mixerTarget->mixer->Mixin(*shaderBytecode, listShaderToMix, errorMsgs);
            if (!success) return error(errorMsgs, "Failed to a mix the shader: " + shaderFullName + " into mixer: " + mixerTarget->name);
        }

        string compositionString = shaderDefinition.compositionString;
        if (compositionString.size() > 0)
        {
            //Directly add compositions into the mixed shader
            success = AddCompositionsToMixer(mixerTarget, compositionString, shaderFullName, callbackRequestDataForShader, mapShaderNameBytecode, mixerMap,
                listAllocatedBytecodes, listUserDefinedMacros, parser, errorMsgs);

            if (!success) 
                return error(errorMsgs, "Failed to add the compositions instruction to the mixer: " + compositionString);
        }
    }

    return true;
}

//===================================================================================================================================
//===================================================================================================================================
// XKFX command lines parsing
bool XkfxParser::ProcessXkfxCommandLines(string effectCmdLines, glslang::CallbackRequestDataForShader callbackRequestDataForShader, vector<string>& errorMsgs)
{
    bool success = true;

    XkslParser* parser = new XkslParser();
    vector<XkslUserDefinedMacro> listUserDefinedMacros;
    vector<SpxBytecode*> listAllocatedBytecodes;
    unordered_map<string, EffectMixerObject*> mixerMap;
    unordered_map<string, SpxBytecode*> mapShaderNameBytecode;

    if (!parser->InitialiseXkslang())
    {
        error(errorMsgs, "Failed to initialize the XkslParser");
        return false;
    }

    string previousPartialLine = "";  //to let us have an instruction defined on several lines
    string parsedLine = "";
    stringstream ss(effectCmdLines);
    while (getline(ss, parsedLine, '\n'))
    {
        parsedLine = XkslangUtils::trim(parsedLine, " \t");
        if (parsedLine.size() == 0) continue;
        if (XkslangUtils::startWith(parsedLine, "//"))
        {
            //a comment: ignore the line
            continue;
        }

        //if some instructions are not complete (some unclosed parentheses or brackets, we concatenate them with the next instructions)
        parsedLine = previousPartialLine + parsedLine;
        if (!IsCommandLineInstructionComplete(parsedLine))
        {
            previousPartialLine = parsedLine;
            continue;
        }
        else previousPartialLine = "";
        
        string instructionFullLine = XkslangUtils::trim(parsedLine);
        string firstInstruction;
        string remainingLine;
        if (!XkfxParser::GetNextInstruction(instructionFullLine, firstInstruction, remainingLine, '(', true))
            return error(errorMsgs, "Failed to get the next instruction from: " + instructionFullLine);

        if (firstInstruction.compare("break") == 0)
        {
            //quit parsing the effect
            break;
        }
        else if (firstInstruction.compare("setDefine") == 0)
        {
            string strMacrosDefinition = XkslangUtils::trim(remainingLine);
            if (strMacrosDefinition.size() == 0) {
                error(errorMsgs, "missing macro definition");
                success = false; break;
            }

            if (XkslParser::ParseStringMacroDefinition(strMacrosDefinition.c_str(), listUserDefinedMacros, false) != 1)
            {
                error(errorMsgs, "Fails to parse the macros definition: " + strMacrosDefinition);
                success = false; break;
            }
        }
        else if (firstInstruction.compare("mixer") == 0)
        {
            string mixerName;
            if (!XkfxParser::GetNextInstruction(remainingLine, mixerName, remainingLine)) {
                error(errorMsgs, "mixer: failed to get the xksl file name");
                success = false; break;
            }
            mixerName = XkslangUtils::trim(mixerName, '\"');

            if (mixerName.find_first_of("<>()[].,+-/*\\?:;\"{}=&%^") != string::npos)
            {
                error(errorMsgs, "Invalid mixer name: " + mixerName);
                success = false; break;
            }

            EffectMixerObject* mixer = CreateAndAddNewMixer(mixerMap, mixerName, errorMsgs);
            if (mixer == nullptr) {
                error(errorMsgs, "Failed to create a new mixer object");
                success = false; break;
            }
        }
        else
        {
            //operation on a mixer (mixer.instructions)
            string mixerName, instruction;
            if (!XkfxParser::SeparateAdotB(firstInstruction, mixerName, instruction)) {
                error(errorMsgs, "Unknown instruction: " + firstInstruction);
                success = false; break;
            }

            if (mixerMap.find(mixerName) == mixerMap.end()) {
                error(errorMsgs, firstInstruction + ": no mixer found with the name:" + mixerName);
                success = false; break;
            }
            EffectMixerObject* mixerTarget = mixerMap[mixerName];

            //get the function parameters
            string instructionParametersStr;
            remainingLine = XkslangUtils::trim(remainingLine);
            if (remainingLine.size() > 0)
            {
                if (!XkslangUtils::getFunctionParameterString(remainingLine, instructionParametersStr)) {
                    instructionParametersStr = "";
                }
            }

            if (instruction.compare("mixin") == 0)
            {
                if (instructionParametersStr.size() == 0) { error(errorMsgs, "Mixin: parameters expected"); success = false; break; }

                success = MixinShaders(instructionParametersStr, mixerTarget, callbackRequestDataForShader, mapShaderNameBytecode, mixerMap, listAllocatedBytecodes, listUserDefinedMacros, parser, errorMsgs);

                if (!success) { error(errorMsgs, "Mixin failed"); success = false; break; }
            }
            else if (instruction.compare("addComposition") == 0)
            {
                if (instructionParametersStr.size() == 0) { error(errorMsgs, "addComposition: parameters expected"); success = false; break; }

                success = AddCompositionsToMixer(mixerTarget, instructionParametersStr, "", callbackRequestDataForShader, mapShaderNameBytecode, mixerMap,
                    listAllocatedBytecodes, listUserDefinedMacros, parser, errorMsgs);

                if (!success) { error(errorMsgs, "Failed to add the compositions instruction to the mixer: " + instructionParametersStr); success = false; break; }
            }
            if (instruction.compare("compile") == 0)
            {
                success = CompileMixer(mixerTarget->mixer, errorMsgs);
                if (!success) { error(errorMsgs, "Failed to compile the effect"); success = false; break; }
            }
            else
            {
                error(errorMsgs, "Unknown mixer instruction: " + instruction);
                success = false; break;
            }
        }

        if (!success) break;
    }

    //Release allocated data
    {
        parser->Finalize();
        delete parser;

        for (auto itm = mixerMap.begin(); itm != mixerMap.end(); itm++)
            delete (*itm).second;

        for (auto itv = listAllocatedBytecodes.begin(); itv != listAllocatedBytecodes.end(); itv++)
            delete (*itv);
    }

    return success;
}
