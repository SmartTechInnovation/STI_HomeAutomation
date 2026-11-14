// Story 1.1 - first unit test: XmlNode_class reads and writes XML without losing
// elements, attributes (names, values, order) or unknown tags.

#include <QtTest>
#include <QFileInfo>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "src/Components/Document/xmlnode.h"

namespace {

/* Reads a whole document; the caller owns the returned root (nullptr on error). */
XmlNode_class *parse(const QByteArray &utf8){
    QXmlStreamReader reader(utf8);
    return XmlNode_class::readDoc(reader);
}

/* Writes a tree as UTF-8 bytes, without auto-formatting (exact bytes compared). */
QByteArray serialise(XmlNode_class *root){
    QByteArray out;
    QXmlStreamWriter writer(&out);
    writer.writeStartDocument();
    root->write(writer);
    writer.writeEndDocument();
    return out;
}

/* Structural equality: tag, attributes in order, children in order. */
bool sameTree(const XmlNode_class *a, const XmlNode_class *b, QString *why){
    if(a->TagName != b->TagName){
        *why = QStringLiteral("tag %1 != %2").arg(a->TagName, b->TagName);
        return false;
    }
    const auto &attrsA = a->attributes();
    const auto &attrsB = b->attributes();
    if(attrsA.size() != attrsB.size()){
        *why = QStringLiteral("<%1>: %2 attributes != %3").arg(a->TagName).arg(attrsA.size()).arg(attrsB.size());
        return false;
    }
    for(int i = 0; i < attrsA.size(); ++i){
        if(attrsA[i]->Name != attrsB[i]->Name || attrsA[i]->Value != attrsB[i]->Value){
            *why = QStringLiteral("<%1>: attribute #%2 %3=\"%4\" != %5=\"%6\"")
                       .arg(a->TagName).arg(i)
                       .arg(attrsA[i]->Name, attrsA[i]->Value, attrsB[i]->Name, attrsB[i]->Value);
            return false;
        }
    }
    const auto &kidsA = a->children();
    const auto &kidsB = b->children();
    if(kidsA.size() != kidsB.size()){
        *why = QStringLiteral("<%1>: %2 children != %3").arg(a->TagName).arg(kidsA.size()).arg(kidsB.size());
        return false;
    }
    for(int i = 0; i < kidsA.size(); ++i){
        if(!sameTree(kidsA[i], kidsB[i], why))
            return false;
    }
    return true;
}

const char *kSample =
    "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
    "<Project IUuid=\"0001\" Title=\"Villa\" ConfigVers=\"1.0.0\" Zeta=\"z\" Alpha=\"a\">\n"
    "    <Program>\n"
    "        <Page IUuid=\"p-1\" Title=\"Garage\">\n"
    "            <BlockInstance IUuid=\"b-1\" Uuid=\"42764a08\" Px=\"300\" Py=\"100\">\n"
    "                <Input ShortName=\"Io\"><Source Ref=\"s-1\"/><Source Ref=\"v-1\"/></Input>\n"
    "                <Output ShortName=\"Op\" IUuid=\"b-1.Op\"/>\n"
    "            </BlockInstance>\n"
    "        </Page>\n"
    "    </Program>\n"
    "    <Foo Bar=\"1\"/>\n"
    "</Project>\n";

} // namespace

class XmlNodeTest : public QObject
{
    Q_OBJECT

private slots:
    void roundTrip_keepsStructureAndAttributeOrder();
    void roundTrip_isIdempotent();
    void unknownElements_areKept();
    void utf8AndEmptyValues_survive();
    void malformedXml_returnsNull();
    void fixturesDir_isReachable();
};

void XmlNodeTest::roundTrip_keepsStructureAndAttributeOrder(){
    QScopedPointer<XmlNode_class> original(parse(kSample));
    QVERIFY(original);

    QScopedPointer<XmlNode_class> reread(parse(serialise(original.data())));
    QVERIFY(reread);

    QString why;
    QVERIFY2(sameTree(original.data(), reread.data(), &why), qPrintable(why));

    // Attribute order is the order of the file, not alphabetical
    const auto &attrs = reread->attributes();
    QStringList names;
    for(const auto *attribute : attrs)
        names << attribute->Name;
    QCOMPARE(names, QStringList({"IUuid", "Title", "ConfigVers", "Zeta", "Alpha"}));

    // Two sources on one input stay two, in order
    XmlNode_class *input = reread->children().at(0)          // Program
                               ->children().at(0)            // Page
                               ->children().at(0)            // BlockInstance
                               ->children().at(0);           // Input
    QCOMPARE(input->TagName, QStringLiteral("Input"));
    QCOMPARE(input->children().size(), 2);
    QCOMPARE(input->children().at(0)->value("Ref"), QStringLiteral("s-1"));
    QCOMPARE(input->children().at(1)->value("Ref"), QStringLiteral("v-1"));
}

void XmlNodeTest::roundTrip_isIdempotent(){
    QScopedPointer<XmlNode_class> first(parse(kSample));
    QVERIFY(first);
    const QByteArray once = serialise(first.data());

    QScopedPointer<XmlNode_class> second(parse(once));
    QVERIFY(second);
    QCOMPARE(serialise(second.data()), once);
}

void XmlNodeTest::unknownElements_areKept(){
    QScopedPointer<XmlNode_class> root(parse(serialise(QScopedPointer<XmlNode_class>(parse(kSample)).data())));
    QVERIFY(root);
    const XmlNode_class *foo = root->children().last();
    QCOMPARE(foo->TagName, QStringLiteral("Foo"));
    QCOMPARE(foo->value("Bar"), QStringLiteral("1"));
}

void XmlNodeTest::utf8AndEmptyValues_survive(){
    const QString unit     = QStringLiteral("°C · m² · Привет · Țară");
    const QByteArray input = QStringLiteral("<Root Unit=\"%1\" Empty=\"\"><Leaf/></Root>").arg(unit).toUtf8();

    QScopedPointer<XmlNode_class> root(parse(input));
    QVERIFY(root);
    QScopedPointer<XmlNode_class> reread(parse(serialise(root.data())));
    QVERIFY(reread);

    QCOMPARE(reread->value("Unit"), unit);
    QVERIFY(reread->hasAttr("Empty"));
    QCOMPARE(reread->value("Empty"), QString());
    QCOMPARE(reread->children().size(), 1);
    QCOMPARE(reread->children().at(0)->TagName, QStringLiteral("Leaf"));
}

void XmlNodeTest::malformedXml_returnsNull(){
    QScopedPointer<XmlNode_class> root(parse("<Project><Page></Project>"));
    QVERIFY(root.isNull());
}

void XmlNodeTest::fixturesDir_isReachable(){
    const QFileInfo readme(QStringLiteral(STIX_FIXTURES_DIR) + QStringLiteral("/README.md"));
    QVERIFY2(readme.exists(), qPrintable(readme.absoluteFilePath()));
}

QTEST_GUILESS_MAIN(XmlNodeTest)
#include "tst_xmlnode.moc"
