#include "sqlstatisticmodel.h"

#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <limits>

static const char *statisticTableName = "statistics";

static void createTable()
{
    if (QSqlDatabase::database().tables().contains(statisticTableName))
        return;

    QSqlQuery query;
    if (!query.exec("CREATE TABLE statistics (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "vocabulary INTEGER NOT NULL, "
                    "date TEXT NOT NULL, "
                    "count INTEGER NOT NULL, "
                    "FOREIGN KEY(vocabulary) REFERENCES vocabularies(id))")) {
       qFatal("Failed to query database: %s", qPrintable(query.lastError().text()));
    }
}

SqlStatisticModel::SqlStatisticModel(QObject *parent) :
    QObject(parent)
{
    createTable();
}

void SqlStatisticModel::updateDailyLearningStatistics(int vocabulary)
{
    int count = getCurrentCount(vocabulary);
    if (count != std::numeric_limits<int>::lowest()) {
        updateStatistics(vocabulary, ++count);
    } else {
        insertStatistics(vocabulary);
    }
}

void SqlStatisticModel::downgradeDailyLearningStatistics(int vocabulary)
{
    int count = getCurrentCount(vocabulary);
    if (count != std::numeric_limits<int>::lowest()) {
        updateStatistics(vocabulary, --count);
    } else {
        insertStatistics(vocabulary, -1);
    }
}

QString SqlStatisticModel::currentDate() const
{
    return QDate::currentDate().toString(Qt::ISODate);
}

int SqlStatisticModel::getCurrentCount(int vocabulary)
{
    QString query = "SELECT count FROM %1 WHERE vocabulary = :vocabulary_id "
            "AND date LIKE :current_date";
    query = query.arg(statisticTableName);
    m_query.prepare(query);
    m_query.bindValue(":vocabulary_id", vocabulary);
    m_query.bindValue(":current_date", currentDate());
    if (!m_query.exec()) {
        qFatal("Failed to query statistics: %s", qPrintable(m_query.lastError().text()));
    }
    if (m_query.first()) {
        return m_query.value(0).toInt();
    }
    return std::numeric_limits<int>::lowest();
}

void SqlStatisticModel::updateStatistics(int vocabulary, int count)
{
    QString query = "UPDATE %1 SET count = :count WHERE vocabulary = :vocabulary_id "
            "AND date LIKE :current_date";
    query = query.arg(statisticTableName);
    m_query.prepare(query);
    m_query.bindValue(":count", count);
    m_query.bindValue(":vocabulary_id", vocabulary);
    m_query.bindValue(":current_date", currentDate());
    if (!m_query.exec()) {
        qFatal("Failed to update statistics: %s", qPrintable(m_query.lastError().text()));
    }
}

void SqlStatisticModel::insertStatistics(int vocabulary, int count)
{
    QString query = "INSERT INTO %1 (vocabulary, date, count) VALUES (:vocabulary_id, "
            ":current_date, :count)";
    query = query.arg(statisticTableName);
    m_query.prepare(query);
    m_query.bindValue(":vocabulary_id", vocabulary);
    m_query.bindValue(":current_date", currentDate());
    m_query.bindValue(":count", count);
    if (!m_query.exec()) {
        qFatal("Failed to create statistics: %s", qPrintable(m_query.lastError().text()));
    }
}

void SqlStatisticModel::removeStatistics(int vocabulary)
{
    QString query = "DELETE FROM %1 WHERE vocabulary = :vocabulary_id "
                    "AND date LIKE :current_date";
    query = query.arg(statisticTableName);
    m_query.prepare(query);
    m_query.bindValue(":vocabulary_id", vocabulary);
    m_query.bindValue(":current_date", currentDate());
    if (!m_query.exec()) {
        qFatal("Failed to create statistics: %s", qPrintable(m_query.lastError().text()));
    }
}

void SqlStatisticModel::removeAllVocabularyStatistics(int vocabulary)
{
    QString query = "DELETE FROM %1 WHERE vocabulary = :vocabulary_id";
    query = query.arg(statisticTableName);
    m_query.prepare(query);
    m_query.bindValue(":vocabulary_id", vocabulary);
    if (!m_query.exec()) {
        qFatal("Failed to create statistics: %s", qPrintable(m_query.lastError().text()));
    }
}

QList<QPair<QDate, int>> SqlStatisticModel::listAllStatistics(int vocabulary)
{
    QString query = "SELECT date, count FROM %1 WHERE vocabulary = :vocabulary_id";
    query = query.arg(statisticTableName);
    m_query.prepare(query);
    m_query.bindValue(":vocabulary_id", vocabulary);
    if (!m_query.exec()) {
        qFatal("Failed to select statistics: %s", qPrintable(m_query.lastError().text()));
    }
    QList<QPair<QDate, int>> statisticList;
    while (m_query.next()) {
        QString dateString = m_query.value(0).toString();
        int count = m_query.value(1).toInt();
        statisticList.append(qMakePair(QDate::fromString(dateString, Qt::ISODate), count));
    }

    return statisticList;
}

