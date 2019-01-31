/*
** Copyright (c) 2014-2019, Eren Okka
**
** This Source Code Form is subject to the terms of the Mozilla Public
** License, v. 2.0. If a copy of the MPL was not distributed with this
** file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

#include <algorithm>

#include <anitomy/token.hpp>

namespace anitomy {

bool Token::operator==(const Token& token) const {
  return type == token.type &&
         value == token.value &&
         enclosed == token.enclosed;
}

////////////////////////////////////////////////////////////////////////////////

static bool CheckTokenFlags(const Token& token, unsigned int flags) {
  auto check_flag = [&flags](unsigned int flag) {
    return (flags & flag) == flag;
  };

  if (flags & kFlagMaskEnclosed) {
    bool success = check_flag(kFlagEnclosed) ? token.enclosed : !token.enclosed;
    if (!success)
      return false;
  }

  if (flags & kFlagMaskCategories) {
    bool success = false;
    auto check_type = [&](TokenFlag fe, TokenFlag fn, TokenType t) {
      if (!success)
        success = check_flag(fe) ? token.type == t :
                  check_flag(fn) ? token.type != t : false;
    };
    check_type(kFlagBracket, kFlagNotBracket, TokenType::Bracket);
    check_type(kFlagDelimiter, kFlagNotDelimiter, TokenType::Delimiter);
    check_type(kFlagIdentifier, kFlagNotIdentifier, TokenType::Identifier);
    check_type(kFlagUnknown, kFlagNotUnknown, TokenType::Unknown);
    check_type(kFlagNotValid, kFlagValid, TokenType::Invalid);
    if (!success)
      return false;
  }

  return true;
}

template<class iterator_t>
static iterator_t FindTokenBase(iterator_t first, iterator_t last,
                                unsigned int flags) {
  return std::find_if(first, last, [&](const Token& token) {
        return CheckTokenFlags(token, flags);
      });
}

Tokens::iterator FindToken(Tokens::iterator first, Tokens::iterator last,
                           unsigned int flags) {
  return FindTokenBase(first, last, flags);
}

Tokens::reverse_iterator FindToken(Tokens::reverse_iterator first,
                                   Tokens::reverse_iterator last,
                                   unsigned int flags) {
  return FindTokenBase(first, last, flags);
}

Tokens::iterator FindPreviousToken(Tokens& tokens,
                                   Tokens::iterator first,
                                   unsigned int flags) {
  auto it = FindToken(std::reverse_iterator<Tokens::iterator>(first),
                       tokens.rend(), flags);
  return it == tokens.rend() ? tokens.end() : (++it).base();
}

Tokens::iterator FindNextToken(Tokens& tokens,
                               Tokens::iterator first,
                               unsigned int flags) {
  return FindToken(++first, tokens.end(), flags);
}

}  // namespace anitomy
