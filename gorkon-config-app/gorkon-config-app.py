#!/bin/env python3

import tkinter as tk
from PIL import Image, ImageTk
from enum import IntEnum

import argparse
import mido
import sys
import os

# Number of encoders (XY = 2 encoders)
ENC_NB      = 8
# Number of buttons
BTN_NB      = 1

REQUEST_REC = 2000

MANU_ID = 0x7D

class SysExMsg(IntEnum):
  PATCH_REQ = 0 # Out: Request for current config
  PATCH_STS = 1 # In:  Send the current config
  PATCH_ENC_CMD = 2 # Out: Change an encoder patch
  PATCH_BTN_CMD = 3 # Out: Change a button patch
  TOGGLE_BTN_CMD = 4 # Out: Change a button toggle
  CHANGE_CHAN_CMD = 5 # Out: Change the midi channel
  SAVE_CMD  = 6 # Out: Save the current config
  RESET_CMD = 7 # Out: Save the current config

class Root:
    midi_in = None
    midi_out = None
    def __init__(self):
        """ Initialization """
        self.root = tk.Tk()
        self.root.title('Gorkon Configuration')
        self.root.resizable(0, 0)

        im = Image.open(os.path.join(sys.path[0], 'gorkon.png'))
        gorkon = ImageTk.PhotoImage(im)
        
        label = tk.Label(self.root, image=gorkon)
        label.image = gorkon
        label.pack()

        input_list  = list(set(mido.get_input_names()))
        output_list = list(set(mido.get_output_names()))
        self.input_conn = tk.StringVar(self.root)
        if len(input_list) > 0:
            self.input_conn.set(input_list[0])
        self.output_conn = tk.StringVar(self.root)
        if len(output_list) > 0:
            self.output_conn.set(output_list[0])

        self.input_conn.trace('w', self.on_change_input_conn)
        self.output_conn.trace('w', self.on_change_output_conn)

        self.ddin = tk.OptionMenu(self.root, self.input_conn, value='')
        self.ddin.place(y=10, w=200)
        self.ddout = tk.OptionMenu(self.root, self.output_conn, value='')
        self.ddout.place(relx=1, y=10, w=200, anchor='ne')

        lbl = tk.Label(text="Channel:")
        lbl.place(y=130)
        self.channel = tk.Entry()
        self.channel.place(y=150, w=50)
        self.channel.bind('<Return>', lambda event: self.on_change_chan(event))

        self.enc_mcc = []

        # Right side text boxes
        pady=300
        for i in range(int(ENC_NB / 2) - 1):
            self.enc_mcc += [tk.Entry()]
            self.enc_mcc[i].place(y=pady, w=50)
            self.enc_mcc[i].bind('<Return>', lambda event, idx=i: self.on_change_cc(event, SysExMsg.PATCH_ENC_CMD, idx))
            pady += 105

        i += 1
        # Joystick y
        self.enc_mcc += [tk.Entry()]
        self.enc_mcc[i].place(x=400, y=700, w=50)
        self.enc_mcc[i].bind('<Return>', lambda event, idx=i: self.on_change_cc(event, SysExMsg.PATCH_ENC_CMD, idx))
        lbl = tk.Label(text=": y")
        lbl.place(x=450, y=700)

        # Left side text boxes
        pady=300
        for i in range(int(ENC_NB / 2), ENC_NB - 1):
            self.enc_mcc += [tk.Entry()]
            self.enc_mcc[i].place(relx=1, y=pady, w=50, anchor='ne')
            self.enc_mcc[i].bind('<Return>',  lambda event, idx=i: self.on_change_cc(event, SysExMsg.PATCH_ENC_CMD, idx))
            pady += 105

        i += 1
        # Joystick x
        self.enc_mcc += [tk.Entry()]
        self.enc_mcc[i].place(x=400, y=680, w=50)
        self.enc_mcc[i].bind('<Return>', lambda event, idx=i: self.on_change_cc(event, SysExMsg.PATCH_ENC_CMD, idx))
        lbl = tk.Label(text=": x")
        lbl.place(x=450, y=680)

        self.btn_mcc = []
        self.var_tog = []

        # Joystick Switch
        self.btn_mcc += [tk.Entry()]
        self.btn_mcc[0].place(x=400, y=720, w=50)
        self.btn_mcc[0].bind('<Return>', lambda event, idx=0: self.on_change_cc(event, SysExMsg.PATCH_BTN_CMD, idx))
        lbl = tk.Label(text=": sw")
        lbl.place(x=450, y=720)
        self.var_tog += [tk.IntVar()]
        self.var_tog[0].trace('w', lambda *args, idx=0: self.on_change_btn_tog(idx))
        btn_tog = tk.Checkbutton(text=': Latch', variable=self.var_tog[0])
        btn_tog.place(x=480, y=720, w=70)

        save_btn = tk.Button(text='Flash the patch', command = self.on_save)
        save_btn.pack()
        
        save_btn = tk.Button(text='Factory reset', command = self.on_reset)
        save_btn.pack()
        
        self.fw_version = tk.StringVar()
        self.fw_version.set('Not Connected.')
        lbl = tk.Label(textvariable=self.fw_version)
        lbl.pack()
        
        # Initialize MIDI ports
        if self.input_conn.get():
            self.midi_in   = mido.open_input(self.input_conn.get(), callback=self.on_midi_receive)
        if self.output_conn.get():
            self.midi_out  = mido.open_output(self.output_conn.get())

        # Initialize timer
        self.update()

        self.root.protocol('WM_DELETE_WINDOW', self.on_close)
        self.root.mainloop()

    def on_save(self):
        # Send the SysEx message
        if self.midi_out:
           self.midi_out.send(mido.Message('sysex', data=[MANU_ID, SysExMsg.SAVE_CMD]))

    def on_reset(self):
        # Send the SysEx message
        if self.midi_out:
           self.midi_out.send(mido.Message('sysex', data=[MANU_ID, SysExMsg.RESET_CMD]))

    def on_change_input_conn(self, *args):
        if self.midi_in:
            self.midi_in.close()
        self.midi_in = mido.open_input(self.input_conn.get(), callback=self.on_midi_receive)

    def on_change_output_conn(self, *args):
        if self.midi_out:
            self.midi_out.close()
        self.midi_out = mido.open_output(self.output_conn.get())

    def on_change_chan(self, e):
        """
        Calback to validate channel input
        Sends the new channel.

        @param e:   event
        """
        if self.midi_out:
            # Get the Entry value
            midi_chan = e.widget.get()
            if int(midi_chan) > 15:
                print("bad channel value: " + midi_chan)
                return
            print('MIDI Channel => ' + midi_chan)

            # Send the SysEx message
            self.midi_out.send(mido.Message('sysex', data=[SysExMsg.CHANGE_CHAN_CMD, int(midi_chan)]))

        # Lose focus to be refreshed by the timer
        self.root.focus()


    def on_change_cc(self, e, msg, idx):
        """
        Calback to validate Entry input
        Sends the new patch for a control.

        @param e:   event
        @param msg: sysex message id (PATCH_ENC_CMD or PATCH_BTN_CMD)
        @param idx: index of the Entry
        """

        if self.midi_out:
            # Get the Entry value
            midi_cc = e.widget.get()
            if int(midi_cc) > 127:
                print("bad CC value: " + midi_cc)
                return
            print(msg.name + '[' + str(idx) + '] => ' + midi_cc)

            # Send the SysEx message
            self.midi_out.send(mido.Message('sysex', data=[MANU_ID, msg, idx, int(midi_cc)]))

        # Lose focus to be refreshed by the timer
        self.root.focus()

    def on_change_btn_tog(self, idx):
        tog = self.var_tog[idx].get()
        print(str(idx) + ' => ' + str(tog))

        # Send the SysEx message
        self.midi_out.send(mido.Message('sysex', data=[MANU_ID, SysExMsg.TOGGLE_BTN_CMD, idx, tog]))

    def on_midi_receive(self, midi_msg):
        """
        Callback to be called on MIDI in event.

        @param midi_msg: The incoming MIDI message
        """

        if (midi_msg.type == 'sysex' and midi_msg.data[0] == MANU_ID and
                                         midi_msg.data[1] == SysExMsg.PATCH_STS):
            ver_maj = midi_msg.data[2]
            ver_min = midi_msg.data[3]
            ver_pat = midi_msg.data[4] + (midi_msg.data[5] << 8)
            self.fw_version.set(f'Firmware: {ver_maj}.{ver_min}.{ver_pat}');
            if self.root.focus_get() != self.channel:
                self.channel.delete(0,"end")
                self.channel.insert(0, midi_msg.data[6])
            enc_mcc = midi_msg.data[7:ENC_NB+7]
            for i in range(ENC_NB):
                # Do not update an Entry that being edited
                if self.root.focus_get() != self.enc_mcc[i]:
                    self.enc_mcc[i].delete(0,"end")
                    self.enc_mcc[i].insert(0, enc_mcc[i])
            btn_cfg = midi_msg.data[ENC_NB+7:ENC_NB+7+BTN_NB*2]
            for i in range(BTN_NB):
                btn_mcc = btn_cfg[i*2]
                btn_tog = btn_cfg[i*2+1]
                # Do not update an Entry that being edited
                if self.root.focus_get() != self.btn_mcc[i]:
                    self.btn_mcc[i].delete(0,"end")
                    self.btn_mcc[i].insert(0, btn_mcc)
                # Remove callback on toggle
                self.var_tog[i].trace_remove(*self.var_tog[i].trace_info()[0])
                self.var_tog[i].set(btn_tog)
                # Set back the callback
                self.var_tog[i].trace('w', lambda *args,idx=i: self.on_change_btn_tog(idx))

    def on_close(self):
        """
        Properly terminate at window closing.
        """

        if self.midi_in:
            self.midi_in.close()
        if self.midi_out:
            self.midi_out.close()

        self.root.destroy()

    def update(self):
        """
        Timer.
        Sends a SysEx message to request for the current patch.
        Update midi input and output lists
        """

        # Send patch request
        if self.midi_out:
            self.midi_out.send(mido.Message('sysex', data=[MANU_ID, SysExMsg.PATCH_REQ]))
        
        # Update MIDI ports
        inmenu = self.ddin['menu']
        inmenu.delete(0, 'end')
        for input in set(mido.get_input_names()):
            inmenu.add_command(label=input,
                command=lambda value=input: self.input_conn.set(value))
        outmenu = self.ddout['menu']
        outmenu.delete(0, 'end')
        for output in set(mido.get_output_names()):
            outmenu.add_command(label=output,
                command=lambda value=output: self.output_conn.set(value))

        self.root.after(REQUEST_REC, self.update)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        prog=sys.argv[0],
        description='This application configures your Gorkon device')
    group = parser.add_mutually_exclusive_group()
    group.add_argument('-a', '--alsa', action='store_true', help='Use ALSA backend.')
    group.add_argument('-j', '--jack', action='store_true', help='Use JACK backend (Default if available).')
    args = parser.parse_args()
    
    is_jack_available = 'UNIX_JACK' in mido.backend.module.get_api_names()
    if not args.alsa and is_jack_available:
        mido.set_backend('mido.backends.rtmidi/UNIX_JACK')
    else:
        mido.set_backend('mido.backends.rtmidi/LINUX_ALSA')
    
    print('Using: ' + mido.backend.api)

    root = Root()
