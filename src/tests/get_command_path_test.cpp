#include "utils/file_utills.hpp"

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

#ifdef _WIN32
#include <process.h>
#define SETENV(name, value) _putenv_s(name, value)
#else
#include <unistd.h>
#define SETENV(name, value) setenv(name, value, 1)
#endif

namespace fs = std::filesystem;

class GetCommandPathTest : public ::testing::Test {
protected:
    void SetUp() override {
        temp_dir = fs::temp_directory_path() / "shell_test_dir";
        fs::create_directories(temp_dir);

        bin_dir = temp_dir / "bin";
        fs::create_directories(bin_dir);

        fake_exe = bin_dir / (std::string("my_test_cmd") + exe_ext);
        std::ofstream(fake_exe).close();

#ifndef _WIN32
        fs::permissions(fake_exe, fs::perms::owner_exec | fs::perms::owner_read | fs::perms::add_perms);
#endif

        SETENV("PATH", bin_dir.string().c_str());
    }

    void TearDown() override {
        fs::remove_all(temp_dir);
    }

    fs::path temp_dir;
    fs::path bin_dir;
    fs::path fake_exe;
#ifdef _WIN32
    std::string exe_ext = ".exe";
#else
    std::string exe_ext = "";
#endif
};


TEST_F(GetCommandPathTest, EmptyCommandReturnsNullopt) {
    EXPECT_FALSE(utils::GetCommandPath("").has_value());
}


TEST_F(GetCommandPathTest, AbsolutePathReturnsPathIfExecutable) {
    auto result = utils::GetCommandPath(fake_exe.string());
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(fs::path(*result), fake_exe);
}


TEST_F(GetCommandPathTest, NonExistentAbsolutePathReturnsNullopt) {
    fs::path non_existent = bin_dir / "definitely_not_there";
    EXPECT_FALSE(utils::GetCommandPath(non_existent.string()).has_value());
}


TEST_F(GetCommandPathTest, FindsCommandInPath) {
    auto result = utils::GetCommandPath("my_test_cmd");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(fs::path(*result).filename(), fake_exe.filename());
}


#ifdef _WIN32
TEST_F(GetCommandPathTest, FindsWindowsExtensions) {
    fs::path bat_file = bin_dir / "script.bat";
    std::ofstream(bat_file).close();

    auto result = utils::GetCommandPath("script");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(fs::path(*result).extension(), ".bat");
}
#endif


TEST_F(GetCommandPathTest, UsesCacheOnSecondCall) {
    auto first = utils::GetCommandPath("my_test_cmd");

    fs::remove(fake_exe);

    auto second = utils::GetCommandPath("my_test_cmd");

    ASSERT_TRUE(second.has_value());
    EXPECT_EQ(first, second);
}


TEST_F(GetCommandPathTest, ReturnsNulloptIfPathIsDirectory) {
    auto result = utils::GetCommandPath(bin_dir.string());
    EXPECT_FALSE(result.has_value());
}