#include "iglobal_context.hpp"
namespace core
{
  IGlobalContext* IGlobalContext::_context;

  void IGlobalContext::SetGlobalContext(IGlobalContext* context)
  {
      _context = context;
  }

  IGlobalContext* IGlobalContext::GetInstance()
  {
      return _context;
  }
}
