//
//  Var2SelectViewController.swift
//  TC_Tracker_mac
//
//  Created by 邓浩 on 2022/6/25.
//

import Cocoa

class Var2SelectViewController: NSViewController, NSComboBoxDataSource {

    @IBOutlet var timeComboBox: NSComboBox!
    @IBOutlet var latComboBox: NSComboBox!
    @IBOutlet var lonComboBox: NSComboBox!
    
    public var ncFilePath = ""
    public var vorOrWindNames: [String] = []
    private var dimsName: [String] = []
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do view setup here.
        
    }
    
    override func viewWillAppear() {
        dimsName = NCFileInfo_Wrapper(ncFilePath: ncFilePath).getVorDimsName(vorOrWindNames[0]).compactMap { $0 as? String }
        
        timeComboBox.dataSource = self
        latComboBox.dataSource = self
        lonComboBox.dataSource = self
        
        timeComboBox.selectItem(at: 0)
        latComboBox.selectItem(at: dimsName.count - 2)
        lonComboBox.selectItem(at: dimsName.count - 1)
//        timeComboBox.stringValue = dimsName[0]
//        latComboBox.stringValue = dimsName[dimsName.count - 2]
//        lonComboBox.stringValue = dimsName[dimsName.count - 1]
    }
    
    
    @IBAction func cancelBtnClicked(_ sender: NSButton) {
        view.window?.close()
    }
    
    @IBAction func okBtnClicked(_ sender: NSButton) {
        if !checkComboBoxes() { return }
        
        let vorName = (vorOrWindNames.count == 1) ? vorOrWindNames[0] : ""
        let uwndName = (vorOrWindNames.count == 1) ? "" : vorOrWindNames[0]
        let vwndName = (vorOrWindNames.count == 1) ? "" : vorOrWindNames[1]
//        guard let mainVC = presentingViewController as? ViewController else { return }
//        mainVC.setVarName(time: timeComboBox.stringValue, lat: latComboBox.stringValue, lon: lonComboBox.stringValue, vor: vorName, u: uwndName, v: vwndName)
        NotificationCenter.default.post(name: NSNotification.Name(rawValue: "AllVarNamesGet"), object: nil, userInfo: ["varNames": [timeComboBox.stringValue, latComboBox.stringValue, lonComboBox.stringValue, vorName, uwndName, vwndName]])
        view.window?.close()
    }
    
    private func checkComboBoxes() -> Bool {
        let comboBoxes = [timeComboBox, latComboBox, lonComboBox]
        
        for comboBox in comboBoxes {
            let selectedIndex = comboBox!.indexOfSelectedItem
            if selectedIndex == -1 {
                let comboBoxValue = comboBox!.stringValue
                let alert = NSAlert()
                alert.messageText = comboBoxValue.isEmpty ? "变量名不能为空！" : "\(comboBoxValue): 该变量名不存在，请重新选择变量名。"
                alert.runModal()
                return false
            }
        }
        return true
    }
    
    func numberOfItems(in comboBox: NSComboBox) -> Int {
        return dimsName.count
    }
    
    func comboBox(_ comboBox: NSComboBox, objectValueForItemAt index: Int) -> Any? {
        return dimsName[index]
    }
    
}
