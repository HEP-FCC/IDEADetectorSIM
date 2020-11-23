#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedtypedef;
#pragma link C++ nestedclass;

#pragma link C++ class RunHeader+;
#pragma link C++ class WaveData+;
#pragma link C++ class std::vector<__uint32_t>+;
#pragma link C++ class std::vector<__uint16_t>+;
#if __cplusplus < 201402L
#pragma link C++ class std::map<__uint16_t, std::vector<__uint16_t> >+;
#else
#pragma link C++ class std::unordered_map<__uint16_t, std::vector<__uint16_t> >+;
#endif
#endif
