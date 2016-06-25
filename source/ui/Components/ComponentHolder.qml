import QtQuick 2.0
import "Snapshots"
import "Output"

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
        else if(comp==2){
            var object=snapshots.createObject(holder);
            actuall.destroy();
            actuall=object;
        }
        else if(comp==3){
            var object=snapshots.createObject(holder);
            actuall.destroy();
            actuall=object;
        }
        else if(comp==4){
            var object=snapshots.createObject(holder);
            actuall.destroy();
            actuall=object;
        }
    }

    //onUsualChanged: {
        //console.info("change");
        //change(usual);
    //}
}
