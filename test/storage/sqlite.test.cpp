#include <mbgl/test/util.hpp>

#include <gtest/gtest.h>
#include <sqlite3.hpp>

TEST(SQLite, Statement) {
    using namespace mbgl;

    mapbox::sqlite::Database db(":memory:", mapbox::sqlite::Create | mapbox::sqlite::ReadWrite);
    db.exec("CREATE TABLE test (id INTEGER);");

    mapbox::sqlite::Statement stmt1 = db.prepare("INSERT INTO test (id) VALUES (?1);");
    ASSERT_EQ(stmt1.lastInsertRowId(), 0);
    ASSERT_EQ(stmt1.changes(), 0u);
    stmt1.bind(1, 10);
    stmt1.run();
    ASSERT_EQ(stmt1.lastInsertRowId(), 1);
    ASSERT_EQ(stmt1.changes(), 1u);

    mapbox::sqlite::Statement stmt2 = db.prepare("INSERT INTO test (id) VALUES (?1);");
    ASSERT_EQ(stmt2.lastInsertRowId(), 0);
    ASSERT_EQ(stmt2.changes(), 0u);
    stmt2.bind(1, 20);
    stmt2.run();
    ASSERT_EQ(stmt2.lastInsertRowId(), 2);
    ASSERT_EQ(stmt2.changes(), 1u);
}
