a token is a sequence of 1 or more characters

there are generic tokens, these can be names that can be assigned a value, for example the names of types, variables, or functions
a generic token can contain the following characters:

alphanumeric: a-z, A-Z, 0-9
special characters: ~!@#$%^&*-_=+?|

while use of many special characters is allowed inside generic tokens, it is recommended to restrict these to operator tokens

excluded: .,;<>(){}[]:"'`/

a generic token can not start with a numeric character

there are special restricted tokens that are one character in length:

. : / - getters
, ; - separators
< >, ( ), { }, [] - braces
" ' ` - literal/string delimiters
