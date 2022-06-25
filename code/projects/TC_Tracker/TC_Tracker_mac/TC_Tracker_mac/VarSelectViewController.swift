//
//  VarSelectViewController.swift
//  TC_Tracker_mac
//
//  Created by 邓浩 on 2022/6/25.
//

import Cocoa

class VarSelectViewController: NSViewController {

    @IBOutlet var vorRadioBtn: NSButton!
    
    @objc dynamic var vorNameIsEnabled = false
    @objc dynamic var windNameIsEnabled = false
    @objc dynamic var windNameLabelColor: NSColor {
        windNameIsEnabled ? .white : .gray
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do view setup here.
    }
    
    @IBAction func radioButtonChanged(_ sender: NSButton) {
        vorNameIsEnabled = (sender.state == vorRadioBtn.state)
        windNameIsEnabled = !vorNameIsEnabled
    }
    
    @IBAction func nextStepBtnClicked(_ sender: NSButton) {
        let secondVC = storyboard?.instantiateController(withIdentifier: "Var2SelectVC") as? Var2SelectViewController
        view.window?.contentViewController = secondVC
    }
    
    
    override class func keyPathsForValuesAffectingValue(forKey key: String) -> Set<String> {
        if key == "windNameLabelColor" {
            return ["windNameIsEnabled"]
        } else {
            return []
        }
    }
    
}
