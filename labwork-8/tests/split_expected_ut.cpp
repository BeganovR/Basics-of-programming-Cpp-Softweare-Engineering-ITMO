#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <processing.h>

#include <expected>

struct Department {
    std::string name;
};

bool operator==(const Department& lhs, const Department& rhs) {
    return lhs.name == rhs.name;
}

std::ostream& operator<<(std::ostream& os, const Department& d) {
    return os << "Department{" << d.name << "}";
}

std::expected<Department, std::string> ParseDepartment(const std::string& str) {
    if (str.empty()) {
        return std::unexpected("Department name is empty");
    }
    if (str.contains(' ')) {
        return std::unexpected("Department name contains space");
    }
    return Department{str};
}

TEST(SplitExpectedTest, SplitExpected) {
    std::vector<std::stringstream> files(1);
    files[0] << "good-department|bad department||another-good-department";

    auto [unexpected_flow, good_flow] = AsDataFlow(files) | Split("|") |
                                        Transform(ParseDepartment) |
                                        SplitExpected(ParseDepartment);

    std::stringstream unexpected_file;
    unexpected_flow | Write(unexpected_file, '.');

    auto expected_result = good_flow | AsVector();

    ASSERT_EQ(unexpected_file.str(),
              "Department name contains space.Department name is empty.");
    ASSERT_THAT(expected_result,
                testing::ElementsAre(Department{"good-department"},
                                     Department{"another-good-department"}));
}