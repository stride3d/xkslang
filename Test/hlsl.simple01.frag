//Test parsing a very simple shader "class"
//Initial glslang parser would not output the new type into SPIR-V unless it is used by a variable
//Since SPIR-X is an intermediary language, we need to output them anyway
//(we force it by declaring an unused variable)

shader ShaderSimple
{
    float4 BaseColor;
};
