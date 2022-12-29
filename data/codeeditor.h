#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QString>

struct CodeEditor {
    CodeEditor() = default;
    ~CodeEditor() = default;

    CodeEditor(const CodeEditor &other) = default;
    CodeEditor(CodeEditor &&other) noexcept = default;

    explicit CodeEditor(int id,
                        const QString &textEditor,
                        const QString &fileFormats,
                        const QString &encoding,
                        bool hasIntellisense,
                        bool hasPlugins, bool canCompile) {
        this->id              = id;
        this->textEditor      = textEditor;
        this->fileFormats     = fileFormats;
        this->encoding        = encoding;
        this->hasIntellisense = hasIntellisense;
        this->hasPlugins      = hasPlugins;
        this->canCompile      = canCompile;
    }

    CodeEditor& operator=(const CodeEditor& other) = default;
    CodeEditor& operator=(CodeEditor&& other) = default;

    int id;
    QString textEditor;
    QString fileFormats;
    QString encoding;
    bool hasIntellisense;
    bool hasPlugins;
    bool canCompile;
};

#endif // CODEEDITOR_H
