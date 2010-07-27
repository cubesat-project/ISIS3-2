#include "Isis.h"

#include <sstream>

#include "Filename.h"
#include "iString.h"
#include "History.h"
#include "Pvl.h"   
#include "TextFile.h"

using namespace Isis;
using namespace std;

void IsisMain() {

  // Get user entered file name & mode
  UserInterface &ui = Application::GetUserInterface();
  Filename fromfile(ui.GetFilename("FROM"));
  string mode = ui.GetString("MODE");

  Filename tofile;
  bool append = false;
  if (ui.WasEntered("TO")) {
    tofile = Filename(ui.GetFilename("TO"));
    append = ui.GetBoolean("APPEND");
  }

  // Extract history from file
  History hist("IsisCube", fromfile.Expanded());
  Pvl pvl = hist.ReturnHist();

  // Print full history
  if(mode == "FULL") {
    if(ui.IsInteractive()) {
      Application::GuiLog(pvl);
    }
    else if (ui.WasEntered("TO")) {
      if (append) {
        pvl.Append(tofile.Expanded());
      }
      else {
        pvl.Write(tofile.Expanded());
      }
    }
    else {
      cout << pvl << endl;
    }
  }

  // Print brief history in command line form
  else if(mode == "BRIEF") {
    TextFile * text = NULL;
    if (ui.WasEntered("TO")) {
      if (append) {
        text = new TextFile(tofile.Expanded(),"append");
      }
      else {
        text = new TextFile(tofile.Expanded(),"overwrite");
      }
    }
    for(int i = 0; i < pvl.Objects(); ++i) {
      string all = pvl.Object(i).Name() + " ";
      PvlGroup user = pvl.Object(i).FindGroup("UserParameters");
      for(int j = 0; j < user.Keywords(); ++j) {
        ostringstream os;
        os << user[j];
        string temp = os.str();
        int index = temp.find("=");
        iString temp1(temp.substr(0, index - 1));
        string temp2 = temp.substr(index + 2);
        all += temp1.DownCase() + "=" + temp2 + " ";
      }
      if(ui.IsInteractive()) {
        Application::GuiLog(all);
      }
      else if (ui.WasEntered("TO")) {
        text->PutLine(all);
      }
      else {
        cout << all << endl;
      }
    }
    if (text) {
      text->Close();
      delete text;
      text = NULL;
    }
  }
}
