#include <mbgl/test/util.hpp>

#include <gtest/gtest.h>
#include <sqlite3.hpp>

TEST(SQLite, Statement) {
    using namespace mbgl;

    mapbox::sqlite::Database db = mapbox::sqlite::Database::open(":memory:", mapbox::sqlite::Create | mapbox::sqlite::ReadWrite);
    db.exec("CREATE TABLE test (id INTEGER);");

    mapbox::sqlite::Statement stmt1{ db, "INSERT INTO test (id) VALUES (?1);" };
    mapbox::sqlite::Query query1{ stmt1 };
    ASSERT_EQ(query1.lastInsertRowId(), 0);
    ASSERT_EQ(query1.changes(), 0u);
    query1.bind(1, 10);
    query1.run();
    ASSERT_EQ(query1.lastInsertRowId(), 1);
    ASSERT_EQ(query1.changes(), 1u);

    mapbox::sqlite::Statement stmt2{ db, "INSERT INTO test (id) VALUES (?1);" };
    mapbox::sqlite::Query query2{ stmt2 };
    ASSERT_EQ(query2.lastInsertRowId(), 0);
    ASSERT_EQ(query2.changes(), 0u);
    query2.bind(1, 20);
    query2.run();
    ASSERT_EQ(query2.lastInsertRowId(), 2);
    ASSERT_EQ(query2.changes(), 1u);
}

TEST(SQLite, TEST_REQUIRES_WRITE(TryOpen)) {
    // Should return a CANTOPEN exception when the database doesn't exist,
    // make sure all the backends behave the same way.
    auto result = mapbox::sqlite::Database::tryOpen("test/fixtures/offline_database/foobar123.db", mapbox::sqlite::ReadOnly);
    ASSERT_TRUE(result.is<mapbox::sqlite::Exception>());
    ASSERT_EQ(result.get<mapbox::sqlite::Exception>().code, mapbox::sqlite::ResultCode::CantOpen);
}
