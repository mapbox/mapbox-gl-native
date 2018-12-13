#include <mbgl/test/util.hpp>
#include <mbgl/test/fixture_log_observer.hpp>

#include <mbgl/storage/sqlite3.hpp>

using namespace mbgl;

TEST(SQLite, Statement) {
    using namespace mbgl;

    mapbox::sqlite::Database db = mapbox::sqlite::Database::open(":memory:", mapbox::sqlite::ReadWriteCreate);
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
    FixtureLog log;

    // Should return a CANTOPEN exception when the database doesn't exist,
    // make sure all the backends behave the same way.
    auto result = mapbox::sqlite::Database::tryOpen("test/fixtures/offline_database/foobar123.db", mapbox::sqlite::ReadOnly);
    ASSERT_TRUE(result.is<mapbox::sqlite::Exception>());
    ASSERT_EQ(result.get<mapbox::sqlite::Exception>().code, mapbox::sqlite::ResultCode::CantOpen);
    EXPECT_EQ(0u, log.uncheckedCount());
}

TEST(SQLite, CloseDatabaseWithPendingTransaction) {
    auto db = std::make_unique<mapbox::sqlite::Database>(mapbox::sqlite::Database::open(
        ":memory:", mapbox::sqlite::ReadWriteCreate));
    mapbox::sqlite::Transaction transaction(*db);
    transaction.commit();
}

TEST(SQLite, CloseMovedDatabaseWithPendingTransaction) {
    // Verifies that we can correctly commit a transaction even if we move the Database object to
    // another address.
    auto db1 = mapbox::sqlite::Database::open(":memory:",
                                              mapbox::sqlite::ReadWriteCreate);
    std::unique_ptr<mapbox::sqlite::Database> db2;
    mapbox::sqlite::Transaction transaction(db1);
    db2 = std::make_unique<mapbox::sqlite::Database>(std::move(db1));
    transaction.commit();
}
