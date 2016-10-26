#pragma once

#include <QObject>
#include <QQmlParserStatus>
#include <QString>
#include <qqml.h>

class Q_DECL_EXPORT QQuickMapboxGLMapParameter : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

public:
    QQuickMapboxGLMapParameter(QObject *parent = 0);
    virtual ~QQuickMapboxGLMapParameter() {};

    int propertyOffset() const { return m_metaPropertyOffset; }

signals:
    void propertyUpdated(const QString &name);

protected:
    // QQmlParserStatus implementation
    void classBegin() override {}
    void componentComplete() override;

private slots:
    void onPropertyUpdated(int index);

private:
    int m_metaPropertyOffset;
};

QML_DECLARE_TYPE(QQuickMapboxGLMapParameter)
