void 
IPL::Parser::error( const location_type &l, const std::string &err_message )
{
      std::cout << "Error at line " << l.begin.line << ": " << err_message <<
 "\n";
//    std::cout << "Error at location " << l << ": " << err_message << "\n";
   exit(1);

}
