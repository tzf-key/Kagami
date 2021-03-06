#include "util.h"

namespace kagami::util {
  Terminator GetTerminatorCode(string src) {
    if (IsBinaryOperator(GetKeywordCode(src))) {
      return kTerminatorBinaryOperator;
    }

    if (IsMonoOperator(GetKeywordCode(src))) {
      return kTerminatorMonoOperator;
    }

    if (src == "=")   return kTerminatorAssign;
    if (src == ",")   return kTerminatorComma;
    if (src == "[")   return kTerminatorLeftBracket;
    if (src == ".")   return kTerminatorDot;
    if (src == "(")   return kTerminatorLeftParen;
    if (src == "]")   return kTerminatorRightSqrBracket;
    if (src == ")")   return kTerminatorRightBracket;
    if (src == "{")   return kTerminatorLeftBrace;
    if (src == "}")   return kTerminatorRightCurBracket;
    if (src == "fn")  return kTerminatorFn;
    if (src == "for") return kTerminatorFor;
    if (src == "in")  return kTerminatorIn;
    if (src == "<-")  return kTerminatorArrow;
    return kTerminatorNull;
  }

  bool IsBinaryOperator(Keyword token) {
    bool result;
    switch (token) {
    case kKeywordBind:
    case kKeywordDeliver:
    case kKeywordPlus:
    case kKeywordMinus:
    case kKeywordTimes:
    case kKeywordDivide:
    case kKeywordEquals:
    case kKeywordLessOrEqual:
    case kKeywordGreaterOrEqual:
    case kKeywordNotEqual:
    case kKeywordGreater:
    case kKeywordLess:
    case kKeywordAnd:
    case kKeywordOr:
    case kKeywordNot:
      result = true;
      break;
    default:
      result = false;
      break;
    }
    return result;
  }

  bool IsMonoOperator(Keyword token) {
    bool result;
    switch (token) {
    case kKeywordNot:
      result = true;
      break;
    default:
      result = false;
      break;
    }

    return result;
  }

  bool IsOperator(Keyword token) {
    return IsBinaryOperator(token) || IsMonoOperator(token);
  }

  int GetTokenPriority(Keyword token) {
    int result;
    switch (token) {
    case kKeywordBind:
      result = 0;
      break;
    case kKeywordAnd:
    case kKeywordOr:
      result = 1;
      break;
    case kKeywordEquals:
    case kKeywordLessOrEqual:
    case kKeywordGreaterOrEqual:
    case kKeywordNotEqual:
    case kKeywordGreater:
    case kKeywordLess:
      result = 2;
      break;
    case kKeywordPlus:
    case kKeywordMinus:
      result = 3;
      break;
    case kKeywordTimes:
    case kKeywordDivide:
      result = 4;
      break;
    default:
      result = 5;
      break;
    }

    return result;
  }

  map<string, Keyword> &GetKeywordBase() {
    using T = pair<string, Keyword>;
    static map<string, Keyword> base = {
      T(kStrAssert         ,kKeywordAssert),
      T(kStrLocal          ,kKeywordLocal),
      T(kStrHash           ,kKeywordHash),
      T(kStrFor            ,kKeywordFor),
      T(kStrIn             ,kKeywordIn),
      T(kStrNullObj        ,kKeywordNullObj),
      T(kStrDestroy        ,kKeywordDestroy),
      T(kStrConvert        ,kKeywordConvert),
      T(kStrRefCount       ,kKeywordRefCount),
      T(kStrTime           ,kKeywordTime),
      T(kStrVersion        ,kKeywordVersion),
      T(kStrCodeNameCmd    ,kKeywordCodeName),
      T(kStrSwap           ,kKeywordSwap),
      T(kStrIf             ,kKeywordIf),
      T(kStrFn             ,kKeywordFn),
      T(kStrEnd            ,kKeywordEnd),
      T(kStrElse           ,kKeywordElse),
      T(kStrElif           ,kKeywordElif),
      T(kStrWhile          ,kKeywordWhile),
      T(kStrPlus           ,kKeywordPlus),
      T(kStrMinus          ,kKeywordMinus),
      T(kStrTimes          ,kKeywordTimes),
      T(kStrDiv            ,kKeywordDivide),
      T(kStrIs             ,kKeywordEquals),
      T(kStrAnd            ,kKeywordAnd),
      T(kStrOr             ,kKeywordOr),
      T(kStrNot            ,kKeywordNot),
      T(kStrLessOrEqual    ,kKeywordLessOrEqual),
      T(kStrGreaterOrEqual ,kKeywordGreaterOrEqual),
      T(kStrNotEqual       ,kKeywordNotEqual),
      T(kStrGreater        ,kKeywordGreater),
      T(kStrLess           ,kKeywordLess),
      T(kStrReturn         ,kKeywordReturn),
      T(kStrContinue       ,kKeywordContinue),
      T(kStrBreak          ,kKeywordBreak),
      T(kStrCase           ,kKeywordCase),
      T(kStrWhen           ,kKeywordWhen),
      T(kStrTypeId         ,kKeywordTypeId),
      T(kStrDir            ,kKeywordDir),
      T(kStrHandle         ,kKeywordHandle),
      T(kStrWait           ,kKeywordWait),
      T(kStrLeave          ,kKeywordLeave)
    };
    return base;
  }

  Keyword GetKeywordCode(string src) {
    auto &base = GetKeywordBase();
    auto it = base.find(src);
    if (it != base.end()) return it->second;
    return kKeywordNull;
  }

  bool IsString(string target) {
    if (target.empty()) return false;
    if (target.size() == 1) return false;
    return(target.front() == '\'' && target.back() == '\'');
  }

  bool IsIdentifier(string target) {
    if (target.empty()) return false;
    const auto head = target.front();

    if (!IsAlpha(head) && head != '_') return false;

    bool result = true;
    for (auto &unit : target) {
      if (!IsDigit(unit) && !IsAlpha(unit) && unit != '_') {
        result = false;
        break;
      }

    }
    return result;
  }

  bool IsInteger(string target) {
    if (target.empty()) return false;
    const auto head = target.front();

    if (compare(head, '-', '+') && target.size() == 1) {
      return false;
    }

    if (!IsDigit(head) && !compare(head, '-', '+')) {
      return false;
    }

    bool result = true;
    for (size_t i = 1; i < target.size(); ++i) {
      if (!IsDigit(target[i])) {
        result = false;
        break;
      }
    }
    return result;
  }

  bool IsFloat(string target) {
    if (target.empty()) return false;
    const auto head = target.front();
    bool dot = false;

    if (compare(head, '-', '+') && target.size() == 1) {
      return false;
    }

    if (!IsDigit(head) && !compare(head, '-', '+')) {
      return false;
    }

    bool result = true;
    for (size_t i = 1; i < target.size(); ++i) {
      if (target[i] == '.' && dot == false) {
        dot = true;
      }
      else if (target[i] == '.' && dot == true) {
        result = false;
        break;
      }

      if (!IsDigit(target[i]) &&
        target[i] != '.') {
        result = false;
        break;
      }

      if (i == target.size() - 1 && !IsDigit(target[i])) {
        result = false;
        break;
      }
    }
    return result;
  }

  bool IsBlank(string target) {
    if (target.empty()) return false;
    bool result = true;
    for (auto &unit : target) {
      if (!compare(unit, ' ', '\t', '\r', '\n')) {
        result = false;
        break;
      }
    }
    return result;
  }


  bool IsSymbol(string target) {
    static const regex kPatternSymbol(R"(==|<-|<=|>=|!=|&&|\|\||[[:Punct:]])");
    if (target.empty()) return false;
    return std::regex_match(target, kPatternSymbol);
  }

  bool IsBoolean(string target) {
    return compare(target, "true", "false");
  }

  StringType GetStringType(string src, bool ignore_symbol_rule) {
    StringType type = kStringTypeNull;
    if (src.empty())              type = kStringTypeNull;
    else if (IsBoolean(src))      type = kStringTypeBool;
    else if (IsIdentifier(src))   type = kStringTypeIdentifier;
    else if (IsInteger(src))      type = kStringTypeInt;
    else if (IsFloat(src))        type = kStringTypeFloat;
    else if (IsBlank(src))        type = kStringTypeBlank;
    else if (IsString(src))       type = kStringTypeString;

    if (!ignore_symbol_rule) {
      if (IsSymbol(src)) type = kStringTypeSymbol;
    }
    return type;
  }

  char GetEscapeChar(char target) {
    char result;
    switch (target) {
    case 't':result = '\t'; break;
    case 'n':result = '\n'; break;
    case 'r':result = '\r'; break;
    default:result = target; break;
    }
    return result;
  }

  wchar_t GetEscapeCharW(wchar_t target) {
    wchar_t result;
    switch (target) {
    case L't':result = L'\t'; break;
    case L'n':result = L'\n'; break;
    case L'r':result = L'\r'; break;
    default:result = target; break;
    }
    return result;
  }

  bool IsWideString(string target) {
    auto result = false;
    for (auto &unit : target) {
      if (unit < 0 || unit > 127) {
        result = true;
        break;
      }
    }
    return result;
  }

  string CombineStringVector(vector<string> target) {
    string result;
    for (size_t i = 0; i < target.size(); ++i) {
      result = result + target[i] + "|";
    }
    result.pop_back();
    return result;
  }

  string GetRawString(string target) {
    string str = target.substr(1, target.size() - 2);
    string output;
    bool escape = false;
    for (size_t i = 0; i < str.size(); i++) {
      if (str[i] == '\\' && !escape) {
        escape = true;
        continue;
      }
      if (escape) {
        output.append(1, GetEscapeChar(str[i]));
        escape = false;
      }
      else {
        output.append(1, str[i]);
      }
    }
    return output;
  }

  string MakeBoolean(bool origin) {
    return origin ? kStrTrue : kStrFalse;
  }

  bool IsDigit(char c) {
    return (c >= '0' && c <= '9');
  }

  bool IsAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
  }

  bool IsPlainType(string type_id) {
    return type_id == kTypeIdInt ||
      type_id == kTypeIdFloat ||
      type_id == kTypeIdString ||
      type_id == kTypeIdBool;
  }
}