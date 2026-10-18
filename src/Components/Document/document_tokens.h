#ifndef DOCUMENT_TOKENS_H
#define DOCUMENT_TOKENS_H

/*
 * Tokens of the program language (structure of Project.xml that the
 * configurator itself understands). Everything else (rooms, users,
 * periphery, ...) is described by NodeType schemas, not by C++ code.
 */

/* ===== Project.xml structure ===== */
#define TOKEN_PROJECT     "Project"
#define TOKEN_ROOT_DEV    "RootDevice"
#define TOKEN_PROGRAM     "Program"
#define TOKEN_PAGE        "Page"
#define TOKEN_BLOCK       "BlockInstance"
#define TOKEN_SIGNAL_IN   "SignalInRef"     // reads a channel / variable  -> 1 output
#define TOKEN_SIGNAL_OUT  "SignalOutRef"    // writes a channel / variable -> 1 input
#define TOKEN_INPUT       "Input"
#define TOKEN_OUTPUT      "Output"
#define TOKEN_PROPERTY    "Property"
#define TOKEN_SOURCE      "Source"

/* ===== Attributes used by the program ===== */
#define ATTR_UUID         "Uuid"            // model / type id
#define ATTR_IUUID        "IUuid"           // instance id
#define ATTR_TITLE        "Title"
#define ATTR_REF          "Ref"             // Source -> Output IUuid
#define ATTR_IO           "Io"              // SignalRef -> channel / variable IUuid
#define ATTR_PX           "Px"
#define ATTR_PY           "Py"
#define ATTR_WIDTH        "Width"
#define ATTR_COLOR        "Color"
#define ATTR_ROOM         "Room"
#define ATTR_CATEGORY     "Category"
#define ATTR_SHORT_NAME   "ShortName"
#define ATTR_VISIBLE      "Visible"
#define ATTR_VALUE        "Value"
#define ATTR_FIXED        "Fixed"
#define ATTR_DATA_TYPE    "DataType"
#define ATTR_CREATION_DATE "CDate"
#define ATTR_MODIFIED_DATE "MDate"
#define ATTR_PAPER_FORMAT "PaperFormat"

/* ===== Schema (NodeType descriptors) ===== */
#define TAG_NODE_TYPE   "NodeType"
#define TAG_PROPERTY    "Property"
#define TAG_VARIANT     "Variant"
#define TAG_CHILD       "Child"
#define TAG_DESCRIPTION "Description"
#define TAG_DOCS        "Docs"
#define TAG_GROUP       "Group"

#define ATTR_TAG_NAME   "TagName"
#define ATTR_TEXT       "Text"
#define ATTR_ICON       "Icon"
#define ATTR_MENU       "MenuName"
#define ATTR_SECTION    "Section"
#define ATTR_IO_TYPE    "IoType"
#define ATTR_MAX        "Max"
#define ATTR_GROUP_BY   "GroupBy"
#define ATTR_TITLE_ATTR "TitleAttr"
#define ATTR_TYPE       "Type"
#define ATTR_DEFAULT    "Default"
#define ATTR_UNIT       "Unit"
#define ATTR_REF        "Ref"
#define ATTR_ELEMENT    "Element"
#define ATTR_GROUP      "Group"
#define ATTR_READ_ONLY  "ReadOnly"
#define ATTR_HIDDEN     "Hidden"
#define ATTR_DATA       "Data"
#define ATTR_AUTO       "Auto"
#define ATTR_MANDATORY  "Mandatory"

#define IO_IN             "In"
#define IO_OUT            "Out"
#define IO_IN_OUT         "InOut"          // variables: read and written on pages

#endif // DOCUMENT_TOKENS_H
