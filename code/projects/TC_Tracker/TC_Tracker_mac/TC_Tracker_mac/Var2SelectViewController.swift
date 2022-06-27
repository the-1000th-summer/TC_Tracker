//
//  Var2SelectViewController.swift
//  TC_Tracker_mac
//
//  Created by 邓浩 on 2022/6/25.
//

import Cocoa

class Var2SelectViewController: NSViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do view setup here.
    }
    
    
    @IBAction func okBtnClicked(_ sender: NSButton) {
        view.window?.performClose(sender)
    }
    
}
