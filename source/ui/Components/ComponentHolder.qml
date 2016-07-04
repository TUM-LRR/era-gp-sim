import QtQuick 2.0
import "Snapshots"
import "Output"
import "editor"
import "Memory"
import "Register"

Item {
    id: holder
    property var usual
    property QtObject actuall: Rectangle{}

    Component{
        id: snapshots
        SnapshotList{
        }
    }

    Component{
        id: output
        Output{
            anchors.fill: parent
        }
    }

    Component{
        id: register
        Register{
            anchors.fill: parent
        }
    }

    Component{
        id: memory
        MemoryComponent{
            anchors.fill: parent
        }
    }

    Component{
        id: editor
        Editor{
            anchors.fill: parent        }
    }

    function change(comp){
        if(comp=="nothing"){
            //console.info(usual);
            change(usual);
        }
        else if(comp=="snapshots"){
            var object=snapshots.createObject(holder);
            actuall.destroy();
            actuall=object;
        }
        else if(comp=="output"){
            var object=output.createObject(holder);
            actuall.destroy();
            actuall=object;
        }
        else if(comp=="editor"){
            var object=editor.createObject(holder);
            actuall.destroy();
            actuall=object;
        }
        else if(comp=="register"){
            var object=register.createObject(holder);
            actuall.destroy();
            actuall=object;
        }
        else if(comp=="memory"){
            var object=memory.createObject(holder);
            actuall.destroy();
            actuall=object;
        }
        else{
            console.info("Unknowen component");
        }
    }

    //onUsualChanged: {
        //console.info("change");
        //change(usual);
    //}
}
