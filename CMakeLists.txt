cmake_minimum_required(VERSION 3.20)
project(dz3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ���� � PostgreSQL
set(PostgreSQL_INCLUDE_DIR "C:/Program Files/PostgreSQL/17/include")
set(PostgreSQL_LIBRARY "C:/Program Files/PostgreSQL/17/lib/libpq.lib")

# ������ ������-������ PostgreSQL::PostgreSQL � ����� libpqxx
add_library(PostgreSQL::PostgreSQL UNKNOWN IMPORTED)
set_target_properties(PostgreSQL::PostgreSQL PROPERTIES
    IMPORTED_LOCATION "${PostgreSQL_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${PostgreSQL_INCLUDE_DIR}"
)

# ���������� libpqxx
add_subdirectory("E:/libpqxx-master" "E:/libpqxx-master/build")

# ������ ����������� ����
add_executable(dz3 dz3.cpp)

# ���������� �����������
target_link_libraries(dz3 PRIVATE pqxx ws2_32)