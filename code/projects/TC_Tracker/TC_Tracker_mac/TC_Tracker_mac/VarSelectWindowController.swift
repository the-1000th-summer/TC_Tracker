//
//  VarSelectWindowController.swift
//  TC_Tracker_mac
//
//  Created by 邓浩 on 2022/6/27.
//

import Cocoa

class VarSelectWindowController: NSWindowController, NSWindowDelegate {

    private var timeVarStr = ""
    private var latVarStr = ""
    private var lonVarStr = ""
    private var vorVarStr = ""
    private var varsName: [String] = []
    
    override func windowDidLoad() {
        super.windowDidLoad()
    
        // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
        
    }
    
    func windowWillClose(_ notification: Notification) {
        NSApp.stopModal()
    }

}
