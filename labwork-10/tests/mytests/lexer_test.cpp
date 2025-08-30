#include <gtest/gtest.h>
#include "../../lib/lexer.h"

// Тест 1: Пустая строка
TEST(LexerTest, EmptyString) {
    Lexer lexer("");
    auto tokens = lexer.Tokenize();
    
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, TokenType::EndOfFile);
}

// Тест 2: Число
TEST(LexerTest, Number) {
    Lexer lexer("123");
    auto tokens = lexer.Tokenize();
    
    ASSERT_EQ(tokens.size(), 2); // число + EOF
    EXPECT_EQ(tokens[0].type, TokenType::Number);
    EXPECT_EQ(tokens[0].value, "123");
}

// Тест 3: Дробное число
TEST(LexerTest, FloatNumber) {
    Lexer lexer("3.14");
    auto tokens = lexer.Tokenize();
    
    ASSERT_EQ(tokens.size(), 2); // число + EOF
    EXPECT_EQ(tokens[0].type, TokenType::Number);
    EXPECT_EQ(tokens[0].value, "3.14");
}

// Тест 4: Строка
TEST(LexerTest, String) {
    Lexer lexer("\"hello\"");
    auto tokens = lexer.Tokenize();
    
    ASSERT_EQ(tokens.size(), 2); // строка + EOF
    EXPECT_EQ(tokens[0].type, TokenType::String);
    EXPECT_EQ(tokens[0].value, "hello");
}

// Тест 5: Идентификатор
TEST(LexerTest, Identifier) {
    Lexer lexer("x");
    auto tokens = lexer.Tokenize();
    
    ASSERT_EQ(tokens.size(), 2); // идентификатор + EOF
    EXPECT_EQ(tokens[0].type, TokenType::Identifier);
    EXPECT_EQ(tokens[0].value, "x");
}

// Тест 6: Ключевое слово
TEST(LexerTest, Keyword) {
    Lexer lexer("if");
    auto tokens = lexer.Tokenize();
    
    ASSERT_EQ(tokens.size(), 2); // ключевое слово + EOF
    EXPECT_EQ(tokens[0].type, TokenType::Keyword);
    EXPECT_EQ(tokens[0].value, "if");
}

// Тест 7: Оператор
TEST(LexerTest, Operator) {
    Lexer lexer("+");
    auto tokens = lexer.Tokenize();
    
    ASSERT_EQ(tokens.size(), 2); // оператор + EOF
    EXPECT_EQ(tokens[0].type, TokenType::Operator);
    EXPECT_EQ(tokens[0].value, "+");
}

// Тест 8: Символ
TEST(LexerTest, Symbol) {
    Lexer lexer("(");
    auto tokens = lexer.Tokenize();
    
    ASSERT_EQ(tokens.size(), 2); // символ + EOF
    EXPECT_EQ(tokens[0].type, TokenType::Symbol);
    EXPECT_EQ(tokens[0].value, "(");
}

// Тест 9: Boolean true
TEST(LexerTest, BooleanTrue) {
    Lexer lexer("true");
    auto tokens = lexer.Tokenize();
    
    ASSERT_EQ(tokens.size(), 2); // boolean + EOF
    EXPECT_EQ(tokens[0].type, TokenType::Boolean);
    EXPECT_EQ(tokens[0].value, "true");
}

// Тест 10: Boolean false
TEST(LexerTest, BooleanFalse) {
    Lexer lexer("false");
    auto tokens = lexer.Tokenize();
    
    ASSERT_EQ(tokens.size(), 2); // boolean + EOF
    EXPECT_EQ(tokens[0].type, TokenType::Boolean);
    EXPECT_EQ(tokens[0].value, "false");
}

// Тест 11: Nil
TEST(LexerTest, NilValue) {
    Lexer lexer("nil");
    auto tokens = lexer.Tokenize();
    
    ASSERT_EQ(tokens.size(), 2); // nil + EOF
    EXPECT_EQ(tokens[0].type, TokenType::Nil);
    EXPECT_EQ(tokens[0].value, "nil");
}

// Тест 12: Простое присваивание
TEST(LexerTest, SimpleAssignment) {
    Lexer lexer("x = 5");
    auto tokens = lexer.Tokenize();
    
    ASSERT_EQ(tokens.size(), 4); // идентификатор + оператор + число + EOF
    EXPECT_EQ(tokens[0].type, TokenType::Identifier);
    EXPECT_EQ(tokens[1].type, TokenType::Operator);
    EXPECT_EQ(tokens[2].type, TokenType::Number);
}

// Тест 13: Простое выражение
TEST(LexerTest, SimpleExpression) {
    Lexer lexer("a + b");
    auto tokens = lexer.Tokenize();
    
    ASSERT_EQ(tokens.size(), 4); // идентификатор + оператор + идентификатор + EOF
    EXPECT_EQ(tokens[0].type, TokenType::Identifier);
    EXPECT_EQ(tokens[1].type, TokenType::Operator);
    EXPECT_EQ(tokens[2].type, TokenType::Identifier);
}

// Тест 14: Несколько ключевых слов
TEST(LexerTest, MultipleKeywords) {
    Lexer lexer("if then else");
    auto tokens = lexer.Tokenize();
    
    ASSERT_EQ(tokens.size(), 4); // if + then + else + EOF
    EXPECT_EQ(tokens[0].type, TokenType::Keyword);
    EXPECT_EQ(tokens[1].type, TokenType::Keyword);
    EXPECT_EQ(tokens[2].type, TokenType::Keyword);
}

// Тест 15: Число с экспонентой
TEST(LexerTest, ExponentNumber) {
    Lexer lexer("1.23e4");
    auto tokens = lexer.Tokenize();
    
    ASSERT_EQ(tokens.size(), 2); // число + EOF
    EXPECT_EQ(tokens[0].type, TokenType::Number);
    EXPECT_EQ(tokens[0].value, "1.23e4");
}

// Тест 16: Составной оператор
TEST(LexerTest, CompoundOperator) {
    Lexer lexer("==");
    auto tokens = lexer.Tokenize();
    
    ASSERT_EQ(tokens.size(), 2); // оператор + EOF
    EXPECT_EQ(tokens[0].type, TokenType::Operator);
    EXPECT_EQ(tokens[0].value, "==");
}

// Тест 17: Скобки
TEST(LexerTest, Parentheses) {
    Lexer lexer("()");
    auto tokens = lexer.Tokenize();
    
    ASSERT_EQ(tokens.size(), 3); // ( + ) + EOF
    EXPECT_EQ(tokens[0].type, TokenType::Symbol);
    EXPECT_EQ(tokens[1].type, TokenType::Symbol);
}

// Тест 18: Квадратные скобки
TEST(LexerTest, Brackets) {
    Lexer lexer("[]");
    auto tokens = lexer.Tokenize();
    
    ASSERT_EQ(tokens.size(), 3); // [ + ] + EOF
    EXPECT_EQ(tokens[0].type, TokenType::Symbol);
    EXPECT_EQ(tokens[1].type, TokenType::Symbol);
}

// Тест 19: Пробелы и переносы строк
TEST(LexerTest, Whitespace) {
    Lexer lexer(" \n\t ");
    auto tokens = lexer.Tokenize();
    
    ASSERT_EQ(tokens.size(), 1); // только EOF
    EXPECT_EQ(tokens[0].type, TokenType::EndOfFile);
}

// Тест 20: Простая функция
TEST(LexerTest, SimpleFunction) {
    Lexer lexer("function()");
    auto tokens = lexer.Tokenize();
    
    ASSERT_EQ(tokens.size(), 4); // function + ( + ) + EOF
    EXPECT_EQ(tokens[0].type, TokenType::Keyword);
    EXPECT_EQ(tokens[0].value, "function");
    EXPECT_EQ(tokens[1].type, TokenType::Symbol);
    EXPECT_EQ(tokens[2].type, TokenType::Symbol);
}