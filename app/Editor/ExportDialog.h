#ifndef EXPORT_DIALOG_H
#define EXPORT_DIALOG_H

#include "../AppState.h"

class ExportDialog
{
public:
    enum class State { CLOSED, OPEN };

    ExportDialog(AppState&);

    void open();

    State update();

private:
    AppState* m_app_state;
    const aiExportFormatDesc* m_selected_format;
    State m_state;
};

#endif
