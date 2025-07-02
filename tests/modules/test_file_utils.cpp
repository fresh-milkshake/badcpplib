#define BADCPPLIB_ENABLE_CORE
#define BADCPPLIB_ENABLE_RESULT
#define BADCPPLIB_ENABLE_FILE_UTILS
#include "badcpplib/badcpplib.hpp"
#include "badcpplib/test_framework.hpp"

using namespace badcpplib;
using namespace badcpplib::testing;

int main() {
    TEST_SUITE("File Utils Module Tests");
    
    TEST_CASE("File read/write operations")
        const string test_file = "test_file_rw.txt";
        const string test_content = "Hello, BadCppLib!\nThis is a test file.\n";
        
        // Write file
        auto write_result = file_utils::write_file(test_file, test_content);
        ASSERT_TRUE(write_result.is_ok());
        
        // Check existence
        ASSERT_TRUE(file_utils::file_exists(test_file));
        
        // Read file
        auto read_result = file_utils::read_file(test_file);
        ASSERT_TRUE(read_result.is_ok());
        ASSERT_EQ(read_result.unwrap(), test_content);
        
        // Get file size
        auto size_result = file_utils::file_size(test_file);
        ASSERT_TRUE(size_result.is_ok());
        ASSERT_EQ(size_result.unwrap(), test_content.size());
        
        // Remove file
        auto remove_result = file_utils::remove_file(test_file);
        ASSERT_TRUE(remove_result.is_ok());
        ASSERT_FALSE(file_utils::file_exists(test_file));
    TEST_END
    
    TEST_CASE("File append operations")
        const string test_file = "test_append.txt";
        const string content1 = "First line\n";
        const string content2 = "Second line\n";
        
        // Create file
        auto write_result = file_utils::write_file(test_file, content1);
        ASSERT_TRUE(write_result.is_ok());
        
        // Add content
        auto append_result = file_utils::append_file(test_file, content2);
        ASSERT_TRUE(append_result.is_ok());
        
        // Check result
        auto read_result = file_utils::read_file(test_file);
        ASSERT_TRUE(read_result.is_ok());
        ASSERT_EQ(read_result.unwrap(), content1 + content2);
        
        // Cleanup
        file_utils::remove_file(test_file);
    TEST_END
    
    TEST_CASE("Line-based file operations")
        const string test_file = "test_lines.txt";
        std::vector<string> test_lines = {
            "Line 1",
            "Line 2",
            "Line 3 with special chars: !@#$%",
            ""  // Empty string
        };
        
        // Write lines
        auto write_result = file_utils::write_lines(test_file, test_lines);
        ASSERT_TRUE(write_result.is_ok());
        
        // Read lines
        auto read_result = file_utils::read_lines(test_file);
        ASSERT_TRUE(read_result.is_ok());
        
        auto read_lines = read_result.unwrap();
        ASSERT_EQ(read_lines.size(), test_lines.size());
        
        for (usize i = 0; i < test_lines.size(); ++i) {
            ASSERT_EQ(read_lines[i], test_lines[i]);
        }
        
        // Cleanup
        file_utils::remove_file(test_file);
    TEST_END
    
    TEST_CASE("File copy and move operations")
        const string source_file = "source.txt";
        const string copy_file = "copy.txt";
        const string move_file = "moved.txt";
        const string test_content = "Content for copy/move test";
        
        // Create source file
        auto write_result = file_utils::write_file(source_file, test_content);
        ASSERT_TRUE(write_result.is_ok());
        
        // Copy
        auto copy_result = file_utils::copy_file(source_file, copy_file);
        ASSERT_TRUE(copy_result.is_ok());
        ASSERT_TRUE(file_utils::file_exists(source_file));
        ASSERT_TRUE(file_utils::file_exists(copy_file));
        
        // Check copy content
        auto read_copy = file_utils::read_file(copy_file);
        ASSERT_TRUE(read_copy.is_ok());
        ASSERT_EQ(read_copy.unwrap(), test_content);
        
        // Move
        auto move_result = file_utils::move_file(source_file, move_file);
        ASSERT_TRUE(move_result.is_ok());
        ASSERT_FALSE(file_utils::file_exists(source_file));
        ASSERT_TRUE(file_utils::file_exists(move_file));
        
        // Check moved file content
        auto read_moved = file_utils::read_file(move_file);
        ASSERT_TRUE(read_moved.is_ok());
        ASSERT_EQ(read_moved.unwrap(), test_content);
        
        // Cleanup
        file_utils::remove_file(copy_file);
        file_utils::remove_file(move_file);
    TEST_END
    
    TEST_CASE("Directory operations")
        const string test_dir = "test_directory";
        const string nested_dir = file_utils::path::join(test_dir, "nested");
        
        // Create directory
        auto create_result = file_utils::create_directory(test_dir);
        ASSERT_TRUE(create_result.is_ok());
        ASSERT_TRUE(file_utils::directory_exists(test_dir));
        
        // Create nested directory
        auto create_nested = file_utils::create_directory(nested_dir);
        ASSERT_TRUE(create_nested.is_ok());
        ASSERT_TRUE(file_utils::directory_exists(nested_dir));
        
        // Create files in directories
        string file1 = file_utils::path::join(test_dir, "file1.txt");
        string file2 = file_utils::path::join(nested_dir, "file2.txt");
        
        file_utils::write_file(file1, "Content 1");
        file_utils::write_file(file2, "Content 2");
        
        // Get file list (non-recursive)
        auto files_result = file_utils::list_files(test_dir, false);
        ASSERT_TRUE(files_result.is_ok());
        auto files = files_result.unwrap();
        ASSERT_EQ(files.size(), 1u); // Only file1.txt
        
        // Get file list (recursive)
        auto recursive_files = file_utils::list_files(test_dir, true);
        ASSERT_TRUE(recursive_files.is_ok());
        auto all_files = recursive_files.unwrap();
        ASSERT_EQ(all_files.size(), 2u); // file1.txt and file2.txt
        
        // Get directory list
        auto dirs_result = file_utils::list_directories(test_dir, false);
        ASSERT_TRUE(dirs_result.is_ok());
        auto dirs = dirs_result.unwrap();
        ASSERT_EQ(dirs.size(), 1u); // Only nested
        
        // Cleanup
        file_utils::remove_file(file1);
        file_utils::remove_file(file2);
        std::filesystem::remove_all(test_dir);
    TEST_END
    
    TEST_CASE("Path utilities")
        string filepath = "/path/to/file.txt";
        
        // filename
        ASSERT_EQ(file_utils::path::filename(filepath), "file.txt");
        
        // extension
        ASSERT_EQ(file_utils::path::extension(filepath), ".txt");
        
        // stem (name without extension)
        ASSERT_EQ(file_utils::path::stem(filepath), "file");
        
        // parent_directory
        ASSERT_EQ(file_utils::path::parent_directory(filepath), "/path/to");
        
        // join
        string joined = file_utils::path::join("/home/user", "documents/file.doc");
        ASSERT_TRUE(joined.find("documents") != string::npos);
        ASSERT_TRUE(joined.find("file.doc") != string::npos);
        
        // normalize
        string complex_path = "/path/./to/../to/file.txt";
        string normalized = file_utils::path::normalize(complex_path);
        ASSERT_TRUE(normalized.find("./") == string::npos);
        ASSERT_TRUE(normalized.find("../") == string::npos);
        
        // is_absolute (platform-dependent tests)
        #ifdef _WIN32
        ASSERT_TRUE(file_utils::path::is_absolute("C:\\absolute\\path"));
        ASSERT_TRUE(file_utils::path::is_absolute("D:/absolute/path"));
        #else
        ASSERT_TRUE(file_utils::path::is_absolute("/absolute/path"));
        #endif
        ASSERT_FALSE(file_utils::path::is_absolute("relative/path"));
        
        // absolute (test with relative path)
        auto abs_result = file_utils::path::absolute("relative_file.txt");
        ASSERT_TRUE(abs_result.is_ok());
        ASSERT_TRUE(file_utils::path::is_absolute(abs_result.unwrap()));
    TEST_END
    
    TEST_CASE("Error handling")
        // Read non-existent file
        auto read_result = file_utils::read_file("nonexistent_file_12345.txt");
        ASSERT_TRUE(read_result.is_err());
        
        // Write to invalid path (different for Windows and Unix)
        auto write_result = file_utils::write_file("", "content");
        ASSERT_TRUE(write_result.is_err());
        
        // Size of non-existent file
        auto size_result = file_utils::file_size("nonexistent_file_12345.txt");
        ASSERT_TRUE(size_result.is_err());
        
        // Remove non-existent file
        auto remove_result = file_utils::remove_file("nonexistent_file_12345.txt");
        ASSERT_TRUE(remove_result.is_err());
        
        // Copy non-existent file
        auto copy_result = file_utils::copy_file("nonexistent.txt", "destination.txt");
        ASSERT_TRUE(copy_result.is_err());
        
        // List files in non-existent directory
        auto list_result = file_utils::list_files("nonexistent_directory_12345");
        ASSERT_TRUE(list_result.is_err());
    TEST_END
    
    TEST_CASE("File existence checks")
        // File does not exist
        ASSERT_FALSE(file_utils::file_exists("definitely_nonexistent_file_12345.txt"));
        
        // Directory does not exist
        ASSERT_FALSE(file_utils::directory_exists("definitely_nonexistent_dir_12345"));
        
        // Create and check temporary file
        const string temp_file = "temp_exist_test.txt";
        file_utils::write_file(temp_file, "test");
        ASSERT_TRUE(file_utils::file_exists(temp_file));
        
        // Create and check temporary directory
        const string temp_dir = "temp_dir_test";
        file_utils::create_directory(temp_dir);
        ASSERT_TRUE(file_utils::directory_exists(temp_dir));
        
        // Cleanup
        file_utils::remove_file(temp_file);
        std::filesystem::remove(temp_dir);
    TEST_END
    
    RUN_ALL_TESTS();
} 