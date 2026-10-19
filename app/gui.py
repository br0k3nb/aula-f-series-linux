"""Native GTK controller, embedded into the executable at build time."""
import colorsys
import io
import json
import math
import os
from pathlib import Path
import re
import shutil
import struct
import subprocess
import sys
import tempfile
import threading
import time

import gi
gi.require_version('Gtk', '3.0')
gi.require_version('Gdk', '3.0')
gi.require_version('GdkPixbuf', '2.0')
gi.require_version('Pango', '1.0')
from gi.repository import Gdk, GdkPixbuf, GLib, Gtk, Pango
import cairo

EFFECTS = ['Off', 'Solid color', 'Breathing', 'Rainbow', 'Flash away', 'Raindrops',
           'Rainbow wheel', 'Ripple', 'Twinkling stars', 'Fade away', 'Snake',
           'Neon stream', 'Reactive', 'Sine wave', 'Scanning', 'Windmill',
           'Waterfall', 'Blossom', 'Rotating storm']
SLEEP_TIMES = [0, 60, 90, 120, 180, 240, 300, 600, 900, 1200]
DEFAULT_PALETTE = ['#FF4D6D', '#FF9F43', '#FFE66D', '#54D99C', '#49CFE7', '#7773FF', '#E177D7']
CSS = b'''
* { font-family: "Inter", "Ubuntu Sans", "Noto Sans", sans-serif; }
window, window.background { background: #0b0d12; color: #e9edf5; }
label { color: #e9edf5; }
label:disabled { color: #5f6880; }
.sidebar { background: #0f1218; border-right: 1px solid #1c212c; padding: 26px 14px 20px 14px; }
.brand { font-size: 24px; font-weight: 800; letter-spacing: 5px; color: #ffffff; }
.brand-mark { background: linear-gradient(135deg, #8b7bff, #3ee0cf); border-radius: 10px;
              min-width: 34px; min-height: 34px; }
.eyebrow { font-size: 10px; font-weight: 700; letter-spacing: 2px; color: #6b7590; }
.accent-text { color: #9d90ff; }
.title { font-size: 30px; font-weight: 800; letter-spacing: -0.5px; color: #ffffff; }
.section-title { font-size: 16px; font-weight: 700; color: #ffffff; }
.row-title { font-size: 14px; font-weight: 600; color: #e3e8f2; }
.subtitle { color: #8b93a7; font-size: 13px; }
.muted { color: #6b7590; font-size: 12px; }
.card { background: #13161e; border: 1px solid #1f2430; border-radius: 18px; padding: 24px;
        box-shadow: 0 10px 30px rgba(0, 0, 0, 0.25); }
.preview-card { background: radial-gradient(ellipse at 50% 0%, #1a1b2e, #11131a 70%); }
.header { background: #0e1016; border-bottom: 1px solid #1a1e27; padding: 12px 28px; }
.device-bar { background: #13161e; border: 1px solid #222734; border-radius: 12px; padding: 3px 3px 3px 12px; }
.device-bar image { color: #9d90ff; }
.device-bar combobox button, .device-bar combobox box button { background: transparent; border: none; box-shadow: none;
        padding: 6px 12px; border-radius: 9px; font-weight: 600; }
.device-bar combobox button:hover { background: rgba(255, 255, 255, 0.05); }
.bar-sep { background: #262c3a; min-width: 1px; margin: 8px 0; }
.pill { border-radius: 999px; padding: 6px 12px 6px 10px; background: rgba(255, 255, 255, 0.04);
        border: 1px solid rgba(255, 255, 255, 0.06); }
.pill label { font-size: 12px; font-weight: 700; color: #8b93a7; }
.pill-dot { border-radius: 6px; min-width: 8px; min-height: 8px; background: #5f6880; }
.pill.ok { background: rgba(62, 224, 160, 0.08); border-color: rgba(62, 224, 160, 0.22); }
.pill.ok label { color: #5fe6b0; }
.pill.ok .pill-dot { background: #3ee0a0; box-shadow: 0 0 8px rgba(62, 224, 160, 0.8); }
.pill.warn { background: rgba(244, 194, 122, 0.08); border-color: rgba(244, 194, 122, 0.25); }
.pill.warn label { color: #f4c27a; }
.pill.warn .pill-dot { background: #f4c27a; }
.chip { font-size: 11px; font-weight: 700; color: #8b93a7; background: rgba(255, 255, 255, 0.04);
        border-radius: 6px; padding: 4px 8px; }
.footer { background: #0f1218; border-top: 1px solid #1c212c; padding: 14px 28px; }
.status-dot { background: #3ee0cf; border-radius: 6px; min-width: 8px; min-height: 8px;
              box-shadow: 0 0 8px rgba(62, 224, 207, 0.8); }
.status-title { font-weight: 700; font-size: 14px; }
button { background: rgba(255, 255, 255, 0.05); background-image: none; border: 1px solid rgba(255, 255, 255, 0.07);
         color: #dfe4ee; border-radius: 10px; padding: 8px 16px; min-height: 20px; box-shadow: none; text-shadow: none;
         font-weight: 600; transition: background 120ms ease, border-color 120ms ease; outline-color: rgba(157, 144, 255, 0.5); }
button label { color: #dfe4ee; }
button:hover { background: rgba(255, 255, 255, 0.09); border-color: rgba(255, 255, 255, 0.12); }
button:active { background: rgba(255, 255, 255, 0.03); }
button:checked { background: rgba(139, 123, 255, 0.16); border-color: rgba(139, 123, 255, 0.45); }
button:checked label { color: #d6cfff; }
button:disabled { opacity: 0.4; }
button.primary { background-image: linear-gradient(180deg, #8e80ff, #6d5cf0); border: 1px solid #7a6af5;
                 padding: 8px 20px; font-weight: 700;
                 box-shadow: inset 0 1px rgba(255, 255, 255, 0.22), 0 4px 14px rgba(109, 92, 240, 0.35); }
button.primary label { color: #ffffff; }
button.primary:hover { background-image: linear-gradient(180deg, #9d91ff, #7b6bf5); }
button.primary:active { background-image: linear-gradient(180deg, #6d5cf0, #6252e0); }
button.primary:disabled { box-shadow: none; }
button.destructive label { color: #ff8a9a; }
button.destructive:hover { background: rgba(255, 92, 120, 0.12); border-color: rgba(255, 92, 120, 0.3); }
button.icon { padding: 8px; min-width: 20px; min-height: 20px; border-radius: 10px; }
button.icon image { color: #c4cbdb; }
button.icon:hover image { color: #ffffff; }
button.nav { background: transparent; border: none; color: #8b93a7; padding: 11px 14px;
             border-radius: 12px; font-weight: 600; }
button.nav label { color: #8b93a7; }
button.nav image { color: #6b7590; }
button.nav:hover { background: #161a23; }
button.nav:hover label { color: #e3e8f2; }
button.nav:checked { background: linear-gradient(90deg, rgba(139, 123, 255, 0.20), rgba(139, 123, 255, 0.04));
                     box-shadow: inset 3px 0 0 #8b7bff; }
button.nav:checked label { color: #ffffff; }
button.nav:checked image { color: #a99dff; }
button.color { padding: 3px; min-height: 24px; min-width: 24px; border-radius: 50%; border: 2px solid transparent; background: transparent; }
button.color:hover { border-color: #e9edf5; }
button.small { padding: 6px; border-radius: 8px; }
entry, spinbutton, combobox button, combobox box button { background: #0f1218; background-image: none; color: #e9edf5;
        border: 1px solid #2a3142; border-radius: 10px; box-shadow: none; }
entry { padding: 8px 12px; min-height: 22px; }
entry:focus, spinbutton:focus { border-color: #8b7bff; box-shadow: 0 0 0 3px rgba(139, 123, 255, 0.18); }
entry selection { background: #5b4dd0; }
spinbutton { padding: 0; }
spinbutton entry { border: none; box-shadow: none; background: transparent; }
spinbutton button { background: transparent; border: none; border-left: 1px solid #2a3142; border-radius: 0; padding: 6px 10px; }
spinbutton button:hover { background: #1a1f2b; }
combobox button { padding: 8px 14px; font-weight: 500; }
combobox button:hover { border-color: #3a4359; background: #141821; }
combobox button label { color: #e9edf5; }
combobox arrow { color: #8b93a7; }
menu, .menu, .context-menu { background: #151922; border: 1px solid #262c3a; border-radius: 12px; padding: 6px; }
menuitem { color: #e3e8f2; padding: 7px 12px; border-radius: 8px; }
menuitem:hover { background: #262044; color: #ffffff; }
menu arrow { color: #8b93a7; }
tooltip, tooltip.background { background: #1d2230; border: 1px solid #2a3142; border-radius: 8px; }
tooltip label { color: #e9edf5; }
scale { padding: 10px 0; }
scale trough { background: #232838; min-height: 6px; border-radius: 6px; border: none; }
scale highlight { background-image: linear-gradient(90deg, #8b7bff, #3ee0cf); border-radius: 6px; border: none; }
scale slider { background: #ffffff; border: none; min-width: 18px; min-height: 18px; margin: -7px;
               border-radius: 50%; box-shadow: 0 0 0 4px rgba(139, 123, 255, 0.25), 0 2px 6px rgba(0, 0, 0, 0.5); }
scale slider:hover { box-shadow: 0 0 0 6px rgba(139, 123, 255, 0.30), 0 2px 6px rgba(0, 0, 0, 0.5); }
scale value { color: #c8bfff; font-weight: 700; min-width: 22px; }
switch { background: #232838; background-image: none; border: 1px solid #2f3647; border-radius: 14px;
         min-width: 46px; min-height: 24px; color: transparent; font-size: 0; }
switch:checked { background-image: linear-gradient(135deg, #8b7bff, #3ee0cf); border-color: transparent; }
switch image { color: transparent; opacity: 0; -gtk-icon-source: none; -gtk-icon-transform: scale(0); }
switch slider { background: #ffffff; background-image: none; border: none; border-radius: 12px;
                min-width: 20px; min-height: 20px; margin: 2px; box-shadow: 0 1px 4px rgba(0, 0, 0, 0.45); }
check { background: #0f1218; background-image: none; border: 1px solid #3a4359; border-radius: 6px;
        min-width: 16px; min-height: 16px; margin-right: 6px; }
check:checked { background-color: #8b7bff; border-color: #8b7bff; color: #0b0d12; }
checkbutton label { color: #c4cbdb; }
expander title { padding: 6px 0; }
expander title label { color: #c4cbdb; font-weight: 600; }
expander title arrow { color: #8b7bff; }
textview, textview text { background: #0b0d12; color: #9fb0cc; font-family: "Ubuntu Sans Mono", monospace; font-size: 12px; }
treeview.view { background: #0f1218; color: #d9e1f0; }
treeview.view:selected { background: #262044; color: #ffffff; }
treeview.view header button { background: #151922; background-image: none; border: none; border-bottom: 1px solid #262c3a;
                              border-radius: 0; color: #8b93a7; font-size: 11px; font-weight: 700; padding: 8px 12px; }
scrolledwindow { border: none; }
.table { border: 1px solid #262c3a; border-radius: 12px; }
scrollbar { background: transparent; border: none; }
scrollbar slider { background: rgba(255, 255, 255, 0.14); min-width: 6px; min-height: 6px; border-radius: 6px; border: none; }
scrollbar slider:hover { background: rgba(255, 255, 255, 0.26); }
separator { background: #1f2430; min-height: 1px; }
.notice { color: #f4c27a; }
.success { color: #5fe6d3; }
.error { color: #ff8a9a; }
list, list row { background: transparent; }
list row:selected { background: #262044; }
progressbar trough { background: #232838; border-radius: 6px; }
progressbar progress { background-image: linear-gradient(90deg, #8b7bff, #3ee0cf); border-radius: 6px; }
'''
# Key bounds, labels and matrix indices recovered from the bundled KB.ini files.
KEY_LAYOUTS = {'F75': [('Esc', 24, 31, 49, 58, 0), ('F1', 98, 31, 123, 58, 12), ('F2', 135, 31, 160, 58, 18), ('F3', 172, 31, 197, 58, 24), ('F4', 210, 32, 235, 59, 30), ('F5', 259, 32, 284, 59, 36), ('F6', 297, 32, 322, 59, 42), ('F7', 335, 32, 360, 59, 48), ('F8', 372, 32, 397, 59, 54), ('F9', 422, 32, 447, 59, 60), ('F10', 460, 32, 485, 59, 66), ('F11', 497, 32, 522, 59, 72), ('F12', 534, 32, 559, 59, 78), ('End', 585, 194, 610, 221, 88), ('`', 23, 81, 48, 108, 1), ('1', 62, 81, 87, 108, 7), ('2', 99, 81, 124, 108, 13), ('3', 137, 81, 162, 108, 19), ('4', 173, 81, 198, 108, 25), ('5', 211, 81, 236, 108, 31), ('6', 249, 81, 274, 108, 37), ('7', 287, 81, 312, 108, 43), ('8', 324, 82, 349, 109, 49), ('9', 361, 82, 386, 109, 55), ('0', 398, 82, 423, 109, 61), ('-', 435, 82, 460, 109, 67), ('=', 473, 82, 498, 109, 73), ('Backspace', 511, 82, 571, 109, 79), ('Delete', 585, 82, 610, 109, 85), ('Tab', 23, 119, 67, 146, 2), ('Q', 81, 119, 106, 146, 8), ('W', 119, 119, 144, 146, 14), ('E', 156, 119, 181, 146, 20), ('R', 193, 119, 218, 146, 26), ('T', 231, 119, 256, 146, 32), ('Y', 268, 119, 293, 146, 38), ('U', 305, 119, 330, 146, 44), ('I', 342, 119, 367, 146, 50), ('O', 380, 119, 405, 146, 56), ('P', 417, 119, 442, 146, 62), ('[', 454, 119, 479, 146, 68), (']', 492, 119, 517, 146, 74), ('Enter', 529, 118, 573, 145, 81), ('PgUp', 585, 119, 610, 146, 86), ('CapsLock', 23, 156, 79, 183, 3), ('A', 89, 156, 114, 183, 9), ('S', 127, 156, 152, 183, 15), ('D', 165, 156, 190, 183, 21), ('F', 202, 156, 227, 183, 27), ('G', 240, 156, 265, 183, 33), ('H', 277, 156, 302, 183, 39), ('J', 314, 156, 339, 183, 45), ('K', 352, 156, 377, 183, 51), ('L', 389, 156, 414, 183, 57), (';', 426, 156, 451, 183, 63), ("'", 463, 156, 488, 183, 69), ('\\', 501, 156, 525, 183, 75), ('LShift', 24, 193, 62, 220, 4), ('Z', 110, 193, 135, 220, 10), ('X', 146, 193, 171, 220, 16), ('C', 183, 193, 208, 220, 22), ('V', 221, 193, 246, 220, 28), ('B', 258, 194, 283, 221, 34), ('N', 296, 193, 321, 220, 40), ('M', 333, 193, 358, 220, 46), (',', 370, 193, 395, 220, 52), ('.', 408, 193, 433, 220, 58), ('K56', 445, 193, 470, 220, 64), ('RShift', 481, 194, 537, 221, 70), ('Up', 547, 193, 572, 220, 82), ('LCtrl', 23, 231, 58, 258, 5), ('LWin', 70, 232, 105, 259, 11), ('LAlt', 116, 232, 151, 259, 17), ('Space', 164, 230, 390, 257, 35), ('FN', 444, 231, 478, 258, 59), ('RAlt', 398, 231, 433, 258, 53), ('Left', 510, 230, 535, 257, 77), ('Down', 547, 230, 572, 257, 83), ('Right', 584, 230, 609, 257, 89), ('PgDn', 585, 157, 610, 184, 87), ('><', 71, 193, 96, 220, 76), ('Wheel', 0, 0, 0, 0, 84)], 'F87': [('Esc', 24, 22, 48, 48, 0), ('F1', 95, 22, 119, 48, 12), ('F2', 130, 22, 154, 48, 18), ('F3', 166, 22, 190, 48, 24), ('F4', 202, 22, 226, 48, 30), ('F5', 256, 22, 280, 48, 36), ('F6', 291, 23, 315, 49, 42), ('F7', 327, 23, 351, 49, 48), ('F8', 362, 23, 386, 49, 54), ('F9', 417, 23, 441, 49, 60), ('F10', 452, 23, 476, 49, 66), ('F11', 488, 23, 512, 49, 72), ('F12', 523, 23, 547, 49, 78), ('`', 23, 64, 47, 90, 1), ('1', 59, 65, 83, 91, 7), ('2', 95, 65, 119, 91, 13), ('3', 131, 65, 155, 91, 19), ('4', 167, 65, 191, 91, 25), ('5', 202, 65, 226, 91, 31), ('6', 238, 65, 262, 91, 37), ('7', 273, 65, 297, 91, 43), ('8', 310, 65, 334, 91, 49), ('9', 346, 65, 370, 91, 55), ('0', 381, 65, 405, 91, 61), ('-', 417, 65, 441, 91, 67), ('=', 452, 65, 476, 91, 73), ('Backspace', 488, 64, 550, 90, 79), ('PrtSc', 567, 22, 591, 48, 84), ('SrcLock', 602, 22, 626, 48, 90), ('Pause', 637, 22, 661, 48, 96), ('Tab', 25, 99, 67, 125, 2), ('Q', 78, 99, 102, 125, 8), ('W', 114, 99, 138, 125, 14), ('E', 149, 99, 173, 125, 20), ('R', 185, 99, 209, 125, 26), ('T', 221, 100, 245, 126, 32), ('Y', 256, 100, 280, 126, 38), ('U', 292, 100, 316, 126, 44), ('I', 328, 100, 352, 126, 50), ('O', 363, 100, 387, 126, 56), ('P', 399, 100, 423, 126, 62), ('[', 435, 100, 459, 126, 68), (']', 470, 100, 494, 126, 74), ('Enter', 507, 100, 549, 126, 81), ('Delete', 567, 99, 591, 125, 86), ('Insert', 567, 64, 591, 90, 85), ('Home', 602, 64, 626, 90, 91), ('PgUp', 638, 64, 662, 90, 97), ('CapsLock', 25, 136, 75, 162, 3), ('A', 87, 136, 111, 162, 9), ('S', 123, 136, 147, 162, 15), ('D', 158, 136, 182, 162, 21), ('F', 194, 136, 218, 162, 27), ('G', 229, 136, 253, 162, 33), ('H', 265, 136, 289, 162, 39), ('J', 301, 136, 325, 162, 45), ('K', 337, 136, 361, 162, 51), ('L', 372, 136, 396, 162, 57), (';', 408, 136, 432, 162, 63), ("'", 444, 136, 468, 162, 69), ('\\', 479, 136, 503, 162, 75), ('End', 602, 100, 626, 126, 92), ('PgDn', 638, 100, 662, 126, 98), ('LShift', 25, 171, 58, 197, 4), ('Z', 105, 172, 129, 198, 10), ('X', 140, 172, 164, 198, 16), ('C', 176, 172, 200, 198, 22), ('V', 212, 172, 236, 198, 28), ('B', 248, 172, 272, 198, 34), ('N', 283, 172, 307, 198, 40), ('M', 319, 172, 343, 198, 46), (',', 355, 172, 379, 198, 52), ('.', 390, 172, 414, 198, 58), ('k56', 426, 172, 450, 198, 64), ('RShift', 462, 171, 549, 197, 82), ('Up', 602, 171, 626, 197, 94), ('RCtrl', 515, 207, 547, 233, 83), ('LCtrl', 25, 207, 57, 233, 5), ('LWin', 69, 207, 101, 233, 11), ('LAlt', 114, 207, 146, 233, 17), ('Space', 159, 208, 369, 234, 35), ('RAlt', 382, 207, 414, 233, 53), ('FN', 426, 207, 458, 233, 59), ('App', 471, 207, 503, 233, 65), ('Left', 567, 208, 591, 234, 89), ('Down', 602, 208, 626, 234, 95), ('Right', 638, 208, 662, 234, 101), ('><', 69, 172, 93, 198, 76)], 'F99': [('Esc', 20, 22, 42, 48, 0), ('F1', 60, 23, 82, 49, 6), ('F2', 92, 23, 114, 49, 12), ('F3', 125, 23, 147, 49, 18), ('F4', 157, 23, 179, 49, 24), ('F5', 197, 22, 219, 48, 30), ('F6', 230, 23, 252, 49, 36), ('F7', 262, 23, 284, 49, 42), ('F8', 295, 23, 317, 49, 48), ('F9', 335, 22, 357, 48, 54), ('F10', 368, 22, 390, 48, 60), ('F11', 399, 23, 421, 49, 66), ('F12', 431, 23, 453, 49, 72), ('`', 20, 67, 42, 93, 1), ('1', 52, 67, 74, 93, 7), ('2', 84, 67, 106, 93, 13), ('3', 116, 67, 138, 93, 19), ('4', 149, 67, 171, 93, 25), ('5', 181, 67, 203, 93, 31), ('6', 213, 67, 235, 93, 37), ('7', 246, 67, 268, 93, 43), ('8', 278, 67, 300, 93, 49), ('9', 310, 67, 332, 93, 55), ('0', 342, 67, 364, 93, 61), ('-', 374, 67, 396, 93, 67), ('=', 406, 67, 428, 93, 73), ('Backspace', 440, 67, 494, 93, 79), ('Delete', 472, 22, 494, 48, 78), ('Home', 519, 23, 541, 49, 90), ('NumLock', 519, 66, 541, 92, 91), ('Num /', 552, 66, 574, 92, 97), ('Num*', 586, 66, 608, 92, 103), ('Num-', 616, 66, 638, 92, 109), ('Tab', 20, 99, 60, 125, 2), ('Q', 69, 98, 91, 124, 8), ('W', 101, 98, 123, 124, 14), ('E', 133, 98, 155, 124, 20), ('R', 166, 98, 188, 124, 26), ('T', 198, 98, 220, 124, 32), ('Y', 230, 98, 252, 124, 38), ('U', 262, 98, 284, 124, 44), ('I', 294, 98, 316, 124, 50), ('O', 326, 98, 348, 124, 56), ('P', 359, 98, 381, 124, 62), ('[', 391, 98, 413, 124, 68), (']', 424, 99, 446, 125, 74), ('Enter', 455, 99, 495, 125, 81), ('End', 551, 23, 573, 49, 96), ('PgUp', 583, 23, 605, 49, 102), ('Num7', 519, 99, 541, 125, 92), ('Num8', 551, 99, 573, 125, 98), ('Num9', 583, 99, 605, 125, 104), ('Num+', 616, 100, 638, 157, 110), ('CapsLock', 20, 132, 68, 158, 3), ('A', 77, 131, 99, 157, 9), ('S', 109, 131, 131, 157, 15), ('D', 141, 131, 163, 157, 21), ('F', 173, 131, 195, 157, 27), ('G', 206, 131, 228, 157, 33), ('H', 238, 131, 260, 157, 39), ('J', 270, 131, 292, 157, 45), ('K', 302, 131, 324, 157, 51), ('L', 334, 131, 356, 157, 57), (';', 368, 131, 390, 157, 63), ("'", 399, 131, 421, 157, 69), ('\\', 431, 131, 453, 157, 75), ('Num4', 519, 131, 541, 157, 93), ('Num5', 550, 131, 572, 157, 99), ('Num6', 583, 131, 605, 157, 105), ('LShift', 20, 163, 50, 189, 4), ('Z', 93, 164, 115, 190, 10), ('X', 125, 164, 147, 190, 16), ('C', 158, 164, 180, 190, 22), ('V', 190, 164, 212, 190, 28), ('B', 222, 164, 244, 190, 34), ('N', 254, 164, 276, 190, 40), ('M', 287, 164, 309, 190, 46), (',', 319, 164, 341, 190, 52), ('.', 351, 164, 373, 190, 58), ('k56', 383, 164, 405, 190, 64), ('RShift', 415, 164, 463, 190, 82), ('Up', 480, 169, 502, 195, 88), ('Num1', 519, 163, 541, 189, 94), ('Num2', 551, 163, 573, 189, 100), ('Num3', 583, 163, 605, 189, 106), ('LCtrl', 21, 195, 51, 221, 5), ('LWin', 61, 196, 91, 222, 11), ('LAlt', 101, 196, 131, 222, 17), ('Space', 140, 197, 334, 223, 35), ('RAlt', 343, 195, 365, 221, 53), ('FN', 376, 195, 398, 221, 59), ('RCtrl', 408, 195, 430, 221, 65), ('Left', 447, 202, 469, 228, 83), ('Down', 479, 202, 501, 228, 89), ('Right', 512, 203, 534, 229, 95), ('Num0', 551, 196, 573, 222, 101), ('Num .', 584, 196, 606, 222, 107), ('Enter', 616, 165, 638, 221, 112), ('PgDn', 615, 22, 637, 48, 108), ('><', 61, 164, 83, 190, 76)]}


def normalize_color(value):
    value = str(value or '').strip().upper()
    if not value.startswith('#'):
        value = '#' + value
    if not re.fullmatch(r'#[0-9A-F]{6}', value):
        raise ValueError('Enter a six-digit color such as #65DFD0.')
    return value


def rgb(value):
    value = normalize_color(value)[1:]
    return tuple(int(value[i:i+2], 16) / 255.0 for i in (0, 2, 4))


REACTIVE_EFFECTS = frozenset((4, 7, 9, 12))
ANIMATED_EFFECTS = frozenset(range(2, 19))
FIXED_COLOR_EFFECTS = frozenset((0, 3, 15, 16, 17))


def lighting_effect_frame(mode, phase, positions, color, palette, multicolor,
                          brightness=1.0, presses=()):
    """Pure local renderer: no device access, clocks, randomness or UI mutations.

    Positions are (matrix index, normalized x, normalized y). Phase already
    includes the speed setting. Reactive presses use that same phase clock.
    These visual models follow each named effect; firmware timing can vary.
    """
    base = rgb(color)
    colors = [rgb(c) for c in palette] or [base]
    brightness = max(0.0, min(1.0, brightness))
    spatial_rainbow = mode in (3, 15, 16, 17)
    rainbow = multicolor or spatial_rainbow
    ordered = sorted(positions, key=lambda p: (round(p[2]*5), p[1] if round(p[2]*5) % 2 == 0 else -p[1]))
    order = {p[0]: i for i, p in enumerate(ordered)}
    count = max(1, len(ordered))

    def chosen(hue):
        if not rainbow:
            return base
        # Firmware-defined rainbow modes use the full spectrum; the others
        # interpolate the user's editable seven-color palette.
        if spatial_rainbow:
            return colorsys.hsv_to_rgb(hue % 1, .95, 1)
        slot = (hue % 1)*len(colors)
        left, blend = int(slot), slot % 1
        a, b = colors[left % len(colors)], colors[(left+1) % len(colors)]
        return tuple(a[c]*(1-blend)+b[c]*blend for c in range(3))

    result = {}
    for index, x, y in positions:
        dx, dy = x-.5, (y-.5)*.48
        angle = math.atan2(dy, dx)/(2*math.pi)
        radius = math.hypot(dx, dy)
        hue, level = phase*.12, 0.0
        if mode == 1:
            level, hue = 1.0, x*.82+y*.12 if multicolor else 0.0
        elif mode == 2:
            level = (.5-.5*math.cos(phase*2*math.pi/3.2))**1.5
            hue = math.floor(phase/3.2)/7
        elif mode == 3:
            level, hue = 1.0, x-phase*.18
        elif mode == 5:
            # Independent falling droplets, with short vertical trails.
            for drop in range(11):
                column = (.127+drop*.61803398875) % 1
                head = ((phase*.47+drop*.371) % 1.65)-.25
                along = head-y
                strength = math.exp(-((x-column)/.055)**2)*max(0.0, 1-abs(along-.06)/.24)
                if strength > level:
                    level, hue = strength, drop/11+phase*.04
        elif mode == 6:
            level, hue = 1.0, angle-phase*.2
            if not rainbow:
                level = .08+.92*(.5+.5*math.cos((angle-phase*.2)*2*math.pi))**2
        elif mode == 8:
            seed = ((index+1)*.61803398875) % 1
            sparkle = (phase*(.27+seed*.22)+seed) % 1
            level, hue = math.exp(-((sparkle-.5)/.095)**2), seed+math.floor(phase*.3)*.17
        elif mode == 10:
            head = (phase*17) % count
            behind = (head-order[index]) % count
            level, hue = max(0.0, 1-behind/14)**1.4, (order[index]/count-phase*.08)
        elif mode == 11:
            wave = ((x+y*.2)-phase*.3) % 1
            level, hue = .12+.88*(.5+.5*math.sin(wave*2*math.pi)), wave
        elif mode == 13:
            curve = .5+.40*math.sin(x*2*math.pi-phase*2)
            level, hue = math.exp(-((y-curve)/.19)**2), x+phase*.12
        elif mode == 14:
            head = .5-.5*math.cos(phase*1.8)
            level, hue = math.exp(-((x-head)/.12)**2), x+phase*.09
        elif mode == 15:
            wheel = (angle-phase*.18)*3
            level, hue = .1+.9*(.5+.5*math.cos(wheel*2*math.pi))**2, angle-phase*.18
        elif mode == 16:
            hue = y-phase*.27+x*.13
            level = .22+.78*(.5+.5*math.sin((y-phase*.5)*2*math.pi))
        elif mode == 17:
            ring = (phase*.3) % .9
            level, hue = math.exp(-((radius-ring)/.12)**2), radius*1.8-phase*.2+angle*.25
        elif mode == 18:
            spiral = angle-radius*1.7-phase*.2
            level, hue = .06+.94*(.5+.5*math.cos(spiral*2*math.pi))**4, spiral
        elif mode in REACTIVE_EFFECTS:
            for key, px, py, pressed_at in presses:
                age = phase-pressed_at
                if age < 0 or age > 3.5:
                    continue
                distance = math.hypot(x-px, (y-py)*.48)
                if mode == 4:
                    strength = math.exp(-((abs(x-px)-age*.62)/.09)**2)*math.exp(-abs(y-py)*13)*math.exp(-age*.9)
                elif mode == 7:
                    strength = math.exp(-((distance-age*.42)/.065)**2)*math.exp(-age*.8)
                elif mode == 9:
                    strength = math.exp(-distance*19)*math.exp(-age*1.4)
                else:
                    strength = math.exp(-age*2.4) if index == key else 0.0
                if strength > level:
                    level, hue = strength, (key*.137+pressed_at*.09) % 1
        selected = chosen(hue)
        result[index] = tuple(max(0.0, min(1.0, c*level*brightness)) for c in selected)
    return result


def rounded(cr, x, y, w, h, r):
    cr.new_sub_path()
    for cx, cy, angle in [(x+w-r, y+r, -90), (x+w-r, y+h-r, 0),
                          (x+r, y+h-r, 90), (x+r, y+r, 180)]:
        cr.arc(cx, cy, r, math.radians(angle), math.radians(angle+90))
    cr.close_path()


class KeyboardPreview(Gtk.EventBox):
    def __init__(self, owner, selectable=False):
        super().__init__()
        self.owner = owner
        self.selectable = selectable
        self.selected = None
        self.bounds = []
        self.image = Gtk.Image()
        self.add(self.image)
        self.render_size = None
        self.set_visible_window(False)
        self.set_size_request(500, 300)
        self.connect('size-allocate', self.allocated)
        if selectable:
            self.add_events(Gdk.EventMask.BUTTON_PRESS_MASK)
            self.connect('button-press-event', self.clicked)
            self.set_tooltip_text('Select a key to edit its assignment.')
        self.set_can_focus(True)
        self.connect('key-press-event', self.preview_typed)

    def do_get_preferred_width(self):
        return 500, 500

    def do_get_preferred_height(self):
        return 300, 300

    def allocated(self, _widget, allocation):
        size = (allocation.width, allocation.height)
        if self.render_size != size:
            self.queue_draw()

    def queue_draw(self):
        allocation = self.get_allocation()
        width, height = max(allocation.width, 500), max(allocation.height, 300)
        self.render_size = (allocation.width, allocation.height)
        surface = cairo.ImageSurface(cairo.FORMAT_ARGB32, width, height)
        self.draw_keyboard(cairo.Context(surface), width, height)
        stream = io.BytesIO()
        surface.write_to_png(stream)
        loader = GdkPixbuf.PixbufLoader.new_with_type('png')
        loader.write(stream.getvalue())
        loader.close()
        self.image.set_from_pixbuf(loader.get_pixbuf())
        Gtk.EventBox.queue_draw(self)

    def clicked(self, _widget, event):
        for label, index, x, y, w, h in self.bounds:
            if x <= event.x <= x+w and y <= event.y <= y+h:
                self.grab_focus()
                if self is getattr(self.owner, 'keyboard_preview', None) and self.owner.lighting_source.get_active_id() == 'effects':
                    self.owner.preview_key_press(index)
                elif self.selectable == 'paint':
                    self.selected = index
                    self.owner.paint_key(label, index)
                else:
                    self.selected = index
                    self.owner.select_key(label, index)
                self.queue_draw()
                break

    def preview_typed(self, _widget, event):
        if self is not getattr(self.owner, 'keyboard_preview', None) or self.owner.lighting_source.get_active_id() != 'effects':
            return False
        name = (Gdk.keyval_name(event.keyval) or '').upper()
        aliases = {'ESCAPE': 'ESC', 'RETURN': 'ENTER', 'SPACE': 'SPACE', 'CONTROL_L': 'LCTRL',
                   'CONTROL_R': 'RCTRL', 'SHIFT_L': 'LSHIFT', 'SHIFT_R': 'RSHIFT'}
        name = aliases.get(name, name)
        for label, index, *_bounds in self.bounds:
            if label.upper() == name:
                self.owner.preview_key_press(index)
                return True
        return False

    def draw_keyboard(self, cr, width, height):
        model = self.owner.model_combo.get_active_id() or 'F75'
        keys = [k for k in KEY_LAYOUTS[model] if k[3] > k[1] and k[4] > k[2]]
        minx, miny = min(k[1] for k in keys), min(k[2] for k in keys)
        maxx, maxy = max(k[3] for k in keys), max(k[4] for k in keys)
        scale = min((width-60)/(maxx-minx), (height-60)/(maxy-miny))
        ox = (width-(maxx-minx)*scale)/2
        oy = (height-(maxy-miny)*scale)/2
        cw, ch = (maxx-minx)*scale, (maxy-miny)*scale
        # Soft drop shadow, then an aluminium-style case with a lit top edge.
        for spread, alpha in ((22, .05), (14, .08), (8, .12)):
            rounded(cr, ox-16-spread/2, oy-10-spread/4, cw+32+spread, ch+34+spread/2, 18+spread/2)
            cr.set_source_rgba(0, 0, 0, alpha)
            cr.fill()
        rounded(cr, ox-16, oy-16, cw+32, ch+32, 16)
        case = cairo.LinearGradient(0, oy-16, 0, oy+ch+16)
        case.add_color_stop_rgb(0, .118, .129, .169)
        case.add_color_stop_rgb(1, .067, .075, .102)
        cr.set_source(case)
        cr.fill_preserve()
        rim = cairo.LinearGradient(0, oy-16, 0, oy+ch+16)
        rim.add_color_stop_rgba(0, 1, 1, 1, .14)
        rim.add_color_stop_rgba(.4, 1, 1, 1, .03)
        rim.add_color_stop_rgba(1, 0, 0, 0, .4)
        cr.set_source(rim)
        cr.set_line_width(1.2)
        cr.stroke()
        rounded(cr, ox-8, oy-8, cw+16, ch+16, 10)
        cr.set_source_rgb(.035, .040, .055)
        cr.fill()
        self.bounds = []
        frame = self.owner.local_preview_frame() if hasattr(self.owner, 'preview_started') else {}
        aliases = {'Backspace': 'Back', 'CapsLock': 'Caps', 'LShift': 'Shift', 'RShift': 'Shift',
                   'LCtrl': 'Ctrl', 'RCtrl': 'Ctrl', 'LWin': 'Win', 'RWin': 'Win',
                   'LAlt': 'Alt', 'RAlt': 'Alt', 'Space': '', 'Left': '\u2190',
                   'Right': '\u2192', 'Up': '\u2191', 'Down': '\u2193', 'Delete': 'Del',
                   'Insert': 'Ins', 'PrintScreen': 'PrtSc', 'NumLock': 'Num', 'ScrollLock': 'ScrLk'}
        lit = []
        for label, x1, y1, x2, y2, index in keys:
            x, y, w, h = ox+(x1-minx)*scale, oy+(y1-miny)*scale, (x2-x1)*scale, (y2-y1)*scale
            self.bounds.append((label, index, x, y, w, h))
            led = frame.get(index, (0.0, 0.0, 0.0))
            if index < 126 and (self.owner.lighting_source.get_active_id() == 'perkey' or self.owner.streaming):
                led = rgb(self.owner.perkey_colors[index])
            lit.append((label, index, x, y, w, h, led))
        # LED bleed between the keycaps, drawn underneath every key.
        for _label, _index, x, y, w, h, led in lit:
            power = max(led)
            if power < .02:
                continue
            cx, cy, radius = x+w/2, y+h/2, max(w, h)*.85
            glow = cairo.RadialGradient(cx, cy, 0, cx, cy, radius)
            glow.add_color_stop_rgba(0, *led, .55)
            glow.add_color_stop_rgba(1, *led, 0)
            cr.set_source(glow)
            cr.rectangle(cx-radius, cy-radius, radius*2, radius*2)
            cr.fill()
        for label, index, x, y, w, h, led in lit:
            selected = self.selectable and self.selected == index
            r = min(5, h/5)
            # Keycap skirt, then the top face. The neutral cap colour fades out
            # as the LED brightens, so lit keys preserve the selected RGB hue.
            unlit = 1-max(led)
            rounded(cr, x, y, w, h, r)
            cr.set_source_rgb(*(.070*unlit+c*.40 for c in led))
            cr.fill()
            inset = max(1.5, h*.09)
            face = cairo.LinearGradient(0, y, 0, y+h)
            face.add_color_stop_rgb(0, *(.165*unlit+c*.80 for c in led))
            face.add_color_stop_rgb(1, *(.115*unlit+c*.62 for c in led))
            rounded(cr, x+inset*.7, y+inset*.4, w-inset*1.4, h-inset*1.5, max(2, r-1))
            cr.set_source(face)
            cr.fill_preserve()
            cr.set_source_rgba(1, 1, 1, .07)
            cr.set_line_width(.8)
            cr.stroke()
            rounded(cr, x, y, w, h, r)
            if max(led) > .02:
                cr.set_source_rgba(*led, .9)
            else:
                cr.set_source_rgba(0, 0, 0, .55)
            cr.set_line_width(.9)
            cr.stroke()
            if selected:
                rounded(cr, x-3, y-3, w+6, h+6, r+3)
                cr.set_source_rgba(.62, .56, 1, .35)
                cr.set_line_width(5)
                cr.stroke_preserve()
                cr.set_source_rgb(.68, .62, 1)
                cr.set_line_width(2)
                cr.stroke()
            text = aliases.get(label, label)
            if len(text) > 7:
                text = text[:6]
            # Ubuntu Sans lacks arrow glyphs; DejaVu covers them.
            font = 'DejaVu Sans' if text in '\u2190\u2191\u2192\u2193' and text else 'Ubuntu Sans'
            cr.select_font_face(font, cairo.FONT_SLANT_NORMAL, cairo.FONT_WEIGHT_BOLD)
            cr.set_font_size(max(7, min(11, h*.30)))
            ext = cr.text_extents(text)
            cr.set_source_rgba(*(.70+c*.30 for c in led), .92)
            cr.move_to(x+(w-ext.width)/2-ext.x_bearing, y+(h-inset)/2-ext.height/2-ext.y_bearing)
            cr.show_text(text)
        return False


class ControlWindow(Gtk.Window):
    def __init__(self, binary):
        super().__init__(title='AULA Studio')
        self.binary = binary
        self.devices = []
        self.busy = False
        self.loading = True
        self.preview_started = time.monotonic()
        self.preview_elapsed = 0.0
        self.preview_last_tick = self.preview_started
        self.preview_paused = False
        self.preview_presses = []
        self.preview_timer = None
        self.color = '#65DFD0'
        self.updating_color_entry = False
        self.palette = list(DEFAULT_PALETTE)
        self.known_fields = set()
        self.touched_fields = set()
        self.saved_snapshot = None
        self.key_index = None
        self.current_node = None
        self.perkey_colors = ['#FFFFFF'] * 126
        self.perkey_loaded = False
        self.streaming = False
        self.stream_stop = None
        self.stream_after_stop = None
        self.recording = False
        self.macros = []
        self.edit_macro_index = None
        self.navigating = False
        self.profiles_path = Path(os.environ.get('XDG_CONFIG_HOME', str(Path.home()/'.config'))) / 'aula-control' / 'profiles.json'
        self.profiles = {}
        self.set_default_size(1150, 820)
        self.set_size_request(920, 680)
        self.connect('destroy', Gtk.main_quit)
        self.connect('destroy', self.on_destroy)
        self.connect('key-press-event', self.record_key, True)
        self.connect('key-release-event', self.record_key, False)
        provider = Gtk.CssProvider()
        provider.load_from_data(CSS)
        Gtk.StyleContext.add_provider_for_screen(self.get_screen(), provider, Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION)
        Gtk.Settings.get_default().set_property('gtk-application-prefer-dark-theme', True)
        shell = Gtk.Box()
        self.add(shell)
        sidebar = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=6)
        sidebar.get_style_context().add_class('sidebar')
        sidebar.set_size_request(210, -1)
        shell.pack_start(sidebar, False, False, 0)
        brandrow = Gtk.Box(spacing=12)
        brandrow.set_margin_start(8)
        brandrow.set_margin_bottom(34)
        mark = Gtk.Box()
        mark.get_style_context().add_class('brand-mark')
        mark.set_valign(Gtk.Align.CENTER)
        brandrow.pack_start(mark, False, False, 0)
        brandtext = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=2)
        brandtext.pack_start(self.label('AULA', 'brand'), False, False, 0)
        studio = self.label('KEYBOARD STUDIO', 'eyebrow')
        studio.set_line_wrap(False)
        brandtext.pack_start(studio, False, False, 0)
        brandrow.pack_start(brandtext, False, False, 0)
        sidebar.pack_start(brandrow, False, False, 0)
        menu_label = self.label('MENU', 'eyebrow')
        menu_label.set_margin_start(14)
        menu_label.set_margin_bottom(6)
        sidebar.pack_start(menu_label, False, False, 0)
        content = Gtk.Box(orientation=Gtk.Orientation.VERTICAL)
        shell.pack_start(content, True, True, 0)
        self.stack = Gtk.Stack()
        self.stack.set_transition_type(Gtk.StackTransitionType.CROSSFADE)
        self.stack.set_transition_duration(150)
        self.nav_buttons = {}
        for title, name, icon in [('Lighting','lighting','display-brightness-symbolic'), ('Keys','keys','input-keyboard-symbolic'),
                                  ('Macros','macros','media-playlist-repeat-symbolic'), ('Studio','studio','media-playback-start-symbolic'),
                                  ('Device','device','preferences-system-symbolic'), ('Profiles','profiles','user-bookmarks-symbolic')]:
            button = Gtk.ToggleButton()
            button.get_style_context().add_class('nav')
            inner = Gtk.Box(spacing=12)
            inner.pack_start(Gtk.Image.new_from_icon_name(icon, Gtk.IconSize.MENU), False, False, 0)
            inner.pack_start(Gtk.Label(label=title, xalign=0), True, True, 0)
            button.add(inner)
            button.connect('clicked', self.navigate, name)
            self.nav_buttons[name] = button
            sidebar.pack_start(button, False, False, 0)
        sidebar.pack_start(Gtk.Box(), True, True, 0)
        foot = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=3)
        foot.set_margin_start(14)
        foot.pack_start(self.label('F SERIES / LINUX', 'eyebrow'), False, False, 0)
        foot.pack_start(self.label('Made for your desktop', 'muted'), False, False, 0)
        sidebar.pack_start(foot, False, False, 0)
        header = Gtk.Box(spacing=12)
        header.get_style_context().add_class('header')
        content.pack_start(header, False, False, 0)
        bar = Gtk.Box(spacing=4)
        bar.get_style_context().add_class('device-bar')
        header.pack_start(bar, False, False, 0)
        bar.pack_start(Gtk.Image.new_from_icon_name('input-keyboard-symbolic', Gtk.IconSize.BUTTON), False, False, 0)
        self.model_combo = Gtk.ComboBoxText()
        self.model_combo.append('', 'Choose model')
        for model in KEY_LAYOUTS:
            self.model_combo.append(model, 'AULA '+model)
        self.model_combo.set_active(0)
        self.model_combo.connect('changed', self.model_changed)
        bar.pack_start(self.model_combo, False, False, 0)
        separator = Gtk.Box()
        separator.get_style_context().add_class('bar-sep')
        bar.pack_start(separator, False, False, 0)
        self.device_combo = Gtk.ComboBoxText()
        cell = self.device_combo.get_cells()[0]
        cell.set_property('ellipsize', Pango.EllipsizeMode.END)
        cell.set_property('width-chars', 30)
        self.device_combo.connect('changed', self.device_changed)
        bar.pack_start(self.device_combo, False, False, 0)
        header.pack_start(Gtk.Box(), True, True, 0)
        self.connection_pill = Gtk.Box(spacing=8)
        self.connection_pill.get_style_context().add_class('pill')
        self.connection_pill.set_valign(Gtk.Align.CENTER)
        dot = Gtk.Box()
        dot.get_style_context().add_class('pill-dot')
        dot.set_valign(Gtk.Align.CENTER)
        self.connection_pill.pack_start(dot, False, False, 0)
        self.connection_label = Gtk.Label(label='Searching\u2026')
        self.connection_pill.pack_start(self.connection_label, False, False, 0)
        header.pack_start(self.connection_pill, False, False, 0)
        self.refresh_button = self.icon_button('view-refresh-symbolic', 'Look for keyboards again', self.refresh)
        self.refresh_button.set_valign(Gtk.Align.CENTER)
        header.pack_start(self.refresh_button, False, False, 0)
        scroller = Gtk.ScrolledWindow()
        scroller.set_policy(Gtk.PolicyType.NEVER, Gtk.PolicyType.AUTOMATIC)
        scroller.add(self.stack)
        content.pack_start(scroller, True, True, 0)
        self.build_lighting()
        self.build_keys()
        self.build_macros()
        self.build_studio()
        self.build_device()
        self.build_profiles()
        footer = Gtk.Box(spacing=12)
        footer.get_style_context().add_class('footer')
        content.pack_start(footer, False, False, 0)
        dot = Gtk.Box()
        dot.get_style_context().add_class('status-dot')
        dot.set_valign(Gtk.Align.CENTER)
        footer.pack_start(dot, False, False, 0)
        statusbox = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=3)
        statusbox.set_valign(Gtk.Align.CENTER)
        footer.pack_start(statusbox, True, True, 0)
        self.save_status = self.label('Ready to connect', 'status-title')
        statusbox.pack_start(self.save_status, False, False, 0)
        self.message = self.label('Connect by USB and choose your keyboard model.', 'subtitle')
        self.message.set_max_width_chars(62)
        statusbox.pack_start(self.message, False, False, 0)
        self.read_button = self.button('Read keyboard', self.read_device)
        self.apply_button = self.button('Apply changes', self.apply, 'primary')
        footer.pack_start(self.read_button, False, False, 0)
        footer.pack_start(self.apply_button, False, False, 0)
        self.loading = False
        self.navigate(self.nav_buttons['lighting'], 'lighting')
        self.show_all()
        self.key_action_changed()
        self.controls()
        self.preview_timer = GLib.timeout_add(33, self.tick_preview)
        GLib.idle_add(self.refresh)

    @staticmethod
    def label(text, style=None):
        widget = Gtk.Label(label=text, xalign=0)
        widget.set_line_wrap(True)
        widget.set_max_width_chars(85)
        if style:
            widget.get_style_context().add_class(style)
        return widget

    def button(self, text, callback, style=None):
        button = Gtk.Button(label=text)
        if style:
            button.get_style_context().add_class(style)
        if callback:
            button.connect('clicked', callback)
        return button

    def icon_button(self, icon, tooltip, callback):
        button = self.button('', callback, 'icon')
        button.remove(button.get_child())
        button.add(Gtk.Image.new_from_icon_name(icon, Gtk.IconSize.BUTTON))
        button.set_tooltip_text(tooltip)
        return button

    def page(self, name, title, subtitle):
        page = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=18)
        page.set_border_width(30)
        self.stack.add_named(page, name)
        top = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=6)
        top.set_margin_bottom(6)
        top.pack_start(self.label(title, 'title'), False, False, 0)
        top.pack_start(self.label(subtitle, 'subtitle'), False, False, 0)
        page.pack_start(top, False, False, 0)
        return page

    def card(self, parent, title=None):
        box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=16)
        box.get_style_context().add_class('card')
        if title:
            box.pack_start(self.label(title, 'section-title'), False, False, 0)
        parent.pack_start(box, False, False, 0)
        return box

    def setting_row(self, parent, title, description, widget):
        row = Gtk.Box(spacing=20)
        text = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=3)
        text.pack_start(self.label(title, 'row-title'), False, False, 0)
        if description:
            text.pack_start(self.label(description, 'subtitle'), False, False, 0)
        row.pack_start(text, True, True, 0)
        widget.set_valign(Gtk.Align.CENTER)
        row.pack_start(widget, False, False, 0)
        parent.pack_start(row, False, False, 0)
        return row

    def build_lighting(self):
        page = self.page('lighting', 'Make it yours.', 'Your keyboard. Your color. Tune the lighting to match your space.')
        # Create the controls before the drawing widget receives its first frame.
        self.effect_combo = Gtk.ComboBoxText()
        for i, name in enumerate(EFFECTS):
            self.effect_combo.append(str(i), name)
        self.effect_combo.set_active(1)
        self.effect_combo.connect('changed', self.changed, 'mode')
        self.brightness = Gtk.Scale.new_with_range(Gtk.Orientation.HORIZONTAL, 0, 9, 1)
        self.brightness.set_value(4)
        self.brightness.set_digits(0)
        self.brightness.set_value_pos(Gtk.PositionType.RIGHT)
        self.brightness.set_size_request(240, -1)
        self.brightness.connect('value-changed', self.changed, 'brightness')
        self.speed = Gtk.Scale.new_with_range(Gtk.Orientation.HORIZONTAL, 0, 4, 1)
        self.speed.set_value(2)
        self.speed.set_digits(0)
        self.speed.set_value_pos(Gtk.PositionType.RIGHT)
        self.speed.set_size_request(240, -1)
        self.speed.connect('value-changed', self.changed, 'speed')
        self.rainbow = Gtk.Switch()
        self.rainbow.connect('notify::active', self.rainbow_changed)
        preview_card = self.card(page)
        preview_card.get_style_context().add_class('preview-card')
        top = Gtk.Box()
        top.pack_start(self.label('LIGHTING PREVIEW', 'eyebrow'), True, True, 0)
        self.lighting_source = Gtk.ComboBoxText()
        self.lighting_source.append('effects', 'Built-in effects')
        self.lighting_source.append('perkey', 'Per-key colors')
        self.lighting_source.set_active(0)
        self.lighting_source.connect('changed', self.lighting_source_changed)
        self.lighting_source.set_valign(Gtk.Align.CENTER)
        top.pack_start(self.lighting_source, False, False, 0)
        top.set_valign(Gtk.Align.CENTER)
        self.preview_model = self.label('F SERIES', 'chip')
        self.preview_model.set_line_wrap(False)
        self.preview_model.set_valign(Gtk.Align.CENTER)
        self.preview_play = self.icon_button('media-playback-pause-symbolic', 'Pause preview', self.toggle_preview)
        self.preview_play.set_valign(Gtk.Align.CENTER)
        top.pack_start(self.preview_model, False, False, 0)
        top.pack_start(self.preview_play, False, False, 10)
        preview_card.pack_start(top, False, False, 0)
        self.keyboard_preview = KeyboardPreview(self, selectable='paint')
        preview_card.pack_start(self.keyboard_preview, True, True, 0)
        self.paint_box = Gtk.Box(spacing=8)
        self.paint_box.set_no_show_all(True)
        self.paint_box.pack_start(self.button('Read key colors', self.read_perkey), False, False, 0)
        self.paint_box.pack_start(self.button('Fill with selected color', lambda *_: self.fill_keys(self.color)), False, False, 0)
        self.paint_box.pack_start(self.button('Clear', lambda *_: self.fill_keys('#000000')), False, False, 0)
        preview_card.pack_start(self.paint_box, False, False, 0)
        self.paint_hint = self.label('Click a key to paint it with your selected color.', 'subtitle')
        self.paint_hint.set_no_show_all(True)
        preview_card.pack_start(self.paint_hint, False, False, 0)
        self.preview_hint = self.label('Animated preview only · Click keys to try reactive effects. Firmware timing may vary.', 'muted')
        preview_card.pack_start(self.preview_hint, False, False, 0)
        settings = self.card(page, 'Lighting controls')
        self.setting_row(settings, 'Effect', 'Choose how your keyboard lights move.', self.effect_combo)
        self.setting_row(settings, 'Brightness', 'Set the intensity of the backlight.', self.brightness)
        self.setting_row(settings, 'Animation speed', 'From a gentle drift to a faster flow.', self.speed)
        settings.pack_start(Gtk.Separator(), False, False, 0)
        self.setting_row(settings, 'Rainbow colors', 'Turn off to use your selected color.', self.rainbow)
        colorrow = Gtk.Box(spacing=12)
        colorrow.pack_start(self.label('Color', 'row-title'), True, True, 0)
        self.color_button = self.button('Choose color', self.choose_color)
        self.color_swatch = Gtk.Image.new_from_pixbuf(self.color_pixbuf(self.color, 26))
        colorrow.pack_start(self.color_swatch, False, False, 0)
        colorrow.pack_start(self.color_button, False, False, 0)
        self.color_entry = Gtk.Entry()
        self.color_entry.set_width_chars(9)
        self.color_entry.set_max_length(7)
        self.color_entry.set_text(self.color)
        self.color_entry.connect('changed', self.color_text_changed)
        self.color_entry.connect('activate', self.color_entered)
        self.color_entry.connect('focus-out-event', self.color_entered)
        colorrow.pack_start(self.color_entry, False, False, 0)
        settings.pack_start(colorrow, False, False, 0)
        swatches = Gtk.Box(spacing=9)
        self.quick_colors = swatches
        quick = self.label('QUICK COLORS', 'eyebrow')
        quick.set_line_wrap(False)
        swatches.pack_start(quick, False, False, 0)
        swatches.pack_start(Gtk.Box(), True, True, 0)
        for color in DEFAULT_PALETTE + ['#FFFFFF']:
            button = self.swatch_button(color, lambda b, c=color: self.set_color(c, True))
            swatches.pack_start(button, False, False, 0)
        settings.pack_start(swatches, False, False, 0)
        palette_expander = Gtk.Expander(label='Advanced · effect color palette')
        self.palette_expander = palette_expander
        palette_box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=12)
        palette_box.set_margin_top(12)
        palette_box.pack_start(self.label('Used when Rainbow colors is on. Choosing a palette color enables it.', 'subtitle'), False, False, 0)
        palette_row = Gtk.Box(spacing=12)
        self.palette_buttons = []
        for index in range(7):
            button = self.swatch_button(self.palette[index], lambda b, i=index: self.choose_palette(i))
            self.palette_buttons.append(button)
            palette_row.pack_start(button, False, False, 0)
        palette_box.pack_start(palette_row, False, False, 0)
        palette_expander.add(palette_box)
        settings.pack_start(palette_expander, False, False, 0)

    def build_device(self):
        page = self.page('device', 'The little details.', 'Adjust behavior and check your wired connection.')
        connected = self.card(page, 'Connection')
        self.device_status = self.label('Looking for AULA interfaces…', 'subtitle')
        connected.pack_start(self.device_status, False, False, 0)
        self.access_button = self.button('Enable wired keyboard access…', self.setup_access)
        self.access_button.set_no_show_all(True)
        connected.pack_start(self.access_button, False, False, 0)
        settings = self.card(page, 'Keyboard behavior')
        self.charging_light = Gtk.Switch()
        self.charging_light.connect('notify::active', self.device_switch_changed, 'charging_light')
        self.charging_hint = self.label('Read the keyboard to discover support for this setting.', 'muted')
        self.setting_row(settings, 'Charging indicator', 'Control the light shown while the battery charges.', self.charging_light)
        settings.pack_start(self.charging_hint, False, False, 0)
        settings.pack_start(Gtk.Separator(), False, False, 0)
        self.sleep_combo = Gtk.ComboBoxText()
        for seconds in SLEEP_TIMES:
            name = 'Never' if not seconds else '{} seconds'.format(seconds) if seconds < 120 else '{} minutes'.format(seconds//60) if seconds%60 == 0 else '{} min {} sec'.format(seconds//60, seconds%60)
            self.sleep_combo.append(str(seconds), name)
        self.sleep_combo.set_active_id('300')
        self.sleep_combo.connect('changed', self.changed, 'sleep_seconds')
        self.setting_row(settings, 'Sleep timer', 'Time without activity before entering sleep.', self.sleep_combo)
        self.debounce = Gtk.SpinButton.new_with_range(1, 5, 1)
        self.debounce.set_value(1)
        self.debounce.connect('value-changed', self.changed, 'debounce_ms')
        self.setting_row(settings, 'Debounce · ms', 'Filter switch chatter between key presses.', self.debounce)
        self.autostart_path = Path(os.environ.get('XDG_CONFIG_HOME', str(Path.home()/'.config'))) / 'autostart' / 'aula-studio.desktop'
        self.autostart = Gtk.Switch()
        self.autostart.set_active(self.autostart_path.exists())
        self.autostart.connect('notify::active', self.autostart_changed)
        self.setting_row(settings, 'Start on login', 'Open AULA Studio when you sign in to your desktop.', self.autostart)
        backup = self.card(page, 'Full keyboard backup')
        backup.pack_start(self.label('Save lighting, all four key layers, custom colors, and stored macros in one file.', 'subtitle'), False, False, 0)
        row = Gtk.Box(spacing=10)
        row.pack_start(self.button('Save full backup…', self.save_backup), False, False, 0)
        row.pack_start(self.button('Restore full backup…', self.restore_backup), False, False, 0)
        backup.pack_start(row, False, False, 0)
        self.output = Gtk.TextView(editable=False, cursor_visible=False)
        self.output.set_wrap_mode(Gtk.WrapMode.WORD_CHAR)
        for side in ('left', 'right', 'top', 'bottom'):
            getattr(self.output, 'set_'+side+'_margin')(12)
        scroll = Gtk.ScrolledWindow()
        scroll.set_min_content_height(170)
        scroll.add(self.output)
        self.diagnostics = Gtk.Expander(label='Connection details and diagnostics')
        self.diagnostics.add(scroll)
        page.pack_start(self.diagnostics, False, False, 0)

    def build_keys(self):
        page = self.page('keys', 'Every key, your way.', 'Choose a key on the keyboard, then assign its action.')
        preview = self.card(page)
        preview.get_style_context().add_class('preview-card')
        self.key_preview = KeyboardPreview(self, selectable=True)
        preview.pack_start(self.key_preview, False, False, 0)
        row = Gtk.Box(spacing=14)
        self.layer_combo = Gtk.ComboBoxText()
        for index, title in enumerate(['Main layer', 'Fn layer 1', 'Fn layer 2', 'Fn layer 3']):
            self.layer_combo.append(str(index), title)
        self.layer_combo.set_active(0)
        row.pack_start(self.layer_combo, True, True, 0)
        self.key_read_button = self.button('Read assignments', self.read_keymap)
        row.pack_start(self.key_read_button, False, False, 0)
        preview.pack_start(row, False, False, 0)
        self.key_card = self.card(page, 'Key assignment')
        self.key_label = self.label('Select a key above', 'subtitle')
        self.key_card.pack_start(self.key_label, False, False, 0)
        self.key_action = Gtk.ComboBoxText()
        for name, title in [('key','Keyboard key'), ('combo','Shortcut'), ('media','Media key'), ('mouse','Mouse action'), ('special','Keyboard function'), ('macro','Stored macro'), ('disable','Disable key')]:
            self.key_action.append(name, title)
        self.key_action.set_active(0)
        self.key_action.connect('changed', self.key_action_changed)
        self.setting_row(self.key_card, 'Action', None, self.key_action)
        self.key_target = Gtk.ComboBoxText()
        self.key_target.set_size_request(230, -1)
        self.key_target_row = self.setting_row(self.key_card, 'Send key', None, self.key_target)
        modrow = Gtk.Grid(column_spacing=12, row_spacing=8)
        self.modifiers = []
        for position, (name, bit) in enumerate([('Left Ctrl',1), ('Left Shift',2), ('Left Alt',4), ('Left Super',8),
                                               ('Right Ctrl',16), ('Right Shift',32), ('Right Alt',64), ('Right Super',128)]):
            button = Gtk.CheckButton(label=name)
            self.modifiers.append((button, bit))
            modrow.attach(button,position % 4, position // 4,1,1)
        self.modifier_row = self.setting_row(self.key_card, 'Modifiers', None, modrow)
        self.code2_combo = Gtk.ComboBoxText()
        self.code2_combo.append('0', 'No second key')
        for code, name in self.normal_keys():
            self.code2_combo.append(str(code),name)
        self.code2_combo.set_active(0)
        self.code2_row = self.setting_row(self.key_card, 'Second key', None, self.code2_combo)
        self.key_message = self.label('Assignments are saved separately from lighting.', 'subtitle')
        self.key_card.pack_start(self.key_message, False, False, 0)
        self.key_apply_button = self.button('Save key assignment', self.apply_key, 'primary')
        self.key_apply_button.set_halign(Gtk.Align.END)
        self.key_card.pack_start(self.key_apply_button, False, False, 0)
        self.keymap = []
        self.key_action_changed()

    @staticmethod
    def normal_keys():
        choices = [(i+4,chr(65+i)) for i in range(26)] + [(i+30,str((i+1)%10)) for i in range(10)]
        choices += [(i+58,'F'+str(i+1)) for i in range(12)] + [(i+104,'F'+str(i+13)) for i in range(12)]
        choices += [(40,'Enter'),(41,'Escape'),(42,'Backspace'),(43,'Tab'),(44,'Space'),(45,'-'),(46,'='),(47,'['),(48,']'),(49,'\\'),(50,'ISO #'),(51,';'),(52,"'"),(53,'`'),(54,','),(55,'.'),(56,'/'),(57,'Caps Lock'),(70,'Print Screen'),(71,'Scroll Lock'),(72,'Pause'),(73,'Insert'),(74,'Home'),(75,'Page Up'),(76,'Delete'),(77,'End'),(78,'Page Down'),(79,'Right'),(80,'Left'),(81,'Down'),(82,'Up'),(83,'Num Lock'),(84,'Numpad /'),(85,'Numpad *'),(86,'Numpad -'),(87,'Numpad +'),(88,'Numpad Enter')]
        choices += [(89+i,'Numpad '+str((i+1)%10)) for i in range(10)]
        choices += [(99,'Numpad .'),(100,'ISO \\'),(101,'Menu'),(224,'Left Ctrl'),(225,'Left Shift'),(226,'Left Alt'),(227,'Left Super'),(228,'Right Ctrl'),(229,'Right Shift'),(230,'Right Alt'),(231,'Right Super')]
        return choices

    def lighting_source_changed(self, *_):
        if not hasattr(self, 'paint_box'):
            return
        custom = self.lighting_source.get_active_id() == 'perkey'
        self.paint_box.set_visible(custom)
        self.paint_hint.set_visible(custom)
        if custom:
            self.paint_box.show_all()
            self.paint_hint.set_text('Read key colors first to preserve your keyboard\u2019s existing custom pattern.' if not self.perkey_loaded else 'Click a key to paint it with your selected color.')
        self.changed(None, 'lighting_source')

    def paint_key(self, label, index):
        if self.lighting_source.get_active_id() != 'perkey' or self.streaming:
            return
        if index >= 126:
            self.paint_hint.set_text('This key has no separately addressable RGB LED.')
            return
        if not self.perkey_loaded:
            self.paint_hint.set_text('Read key colors before editing individual keys, or use Fill to start a new pattern.')
            return
        self.perkey_colors[index] = self.color
        self.paint_hint.set_text(label+' painted '+self.color+'. Apply changes to save your pattern.')
        self.changed(None, 'perkey_colors')

    def fill_keys(self, color):
        self.perkey_colors = [normalize_color(color)] * 126
        self.perkey_loaded = True
        self.paint_hint.set_text('Pattern updated. Click keys to paint individual colors, then Apply changes.')
        self.changed(None, 'perkey_colors')

    def read_perkey(self, *_):
        device = self.selected_device()
        if not device:
            return
        def complete(result):
            colors = result.get('colors')
            if not isinstance(colors,list) or len(colors) != 126:
                raise ValueError('Expected 126 RGB key colors.')
            self.perkey_colors = [normalize_color(c) for c in colors]
            self.perkey_loaded = True
            self.paint_hint.set_text('Current key colors loaded. Choose a color, then click keys to paint.')
            self.message.set_text('Custom key colors loaded from the keyboard.')
            self.keyboard_preview.queue_draw()
        self.run_backend(['--perkey',device['node']], 'Reading individual key colors\u2026', complete)

    def build_macros(self):
        page = self.page('macros', 'Make more happen.', 'Build sequences of keyboard and mouse actions, then assign them to a key.')
        stored = self.card(page, 'Stored on your keyboard')
        row = Gtk.Box(spacing=10)
        self.macro_combo = Gtk.ComboBoxText()
        self.macro_combo.set_hexpand(True)
        row.pack_start(self.macro_combo, True, True, 0)
        row.pack_start(self.button('Read macros', self.read_macros), False, False, 0)
        row.pack_start(self.button('Open', self.open_macro), False, False, 0)
        row.pack_start(self.button('Delete', self.delete_macro, 'destructive'), False, False, 0)
        stored.pack_start(row, False, False, 0)
        self.macro_usage = self.label('Read macros to see the available storage.', 'subtitle')
        stored.pack_start(self.macro_usage, False, False, 0)
        editor = self.card(page, 'Macro editor')
        self.macro_name = Gtk.Entry()
        self.macro_name.set_placeholder_text('Macro name')
        self.macro_name.set_max_length(30)
        self.setting_row(editor, 'Name', None, self.macro_name)
        recordrow = Gtk.Box(spacing=10)
        self.record_button = Gtk.ToggleButton(label='\u25cf  Record keyboard')
        self.record_button.connect('toggled', self.toggle_recording)
        recordrow.pack_start(self.record_button, False, False, 0)
        recordrow.pack_start(self.button('Clear events', lambda *_: self.macro_events.clear()), False, False, 0)
        recordrow.pack_start(self.button('Import\u2026', self.import_macro), False, False, 0)
        recordrow.pack_start(self.button('Export\u2026', self.export_macro), False, False, 0)
        editor.pack_start(recordrow, False, False, 0)
        self.record_hint = self.label('Recording captures keys while this window has focus. Escape stops recording.', 'subtitle')
        editor.pack_start(self.record_hint, False, False, 0)
        self.macro_events = Gtk.ListStore(int,int,bool,int)
        self.event_view = Gtk.TreeView(model=self.macro_events)
        self.event_view.set_headers_visible(True)
        kind_names = ['Keyboard','Modifier','Mouse button','Move X','Move Y','Wheel']
        for column, title in enumerate(['Action','Code','Press / release','Delay (ms)']):
            renderer = Gtk.CellRendererText()
            if column in (1,3):
                renderer.set_property('editable',True)
                renderer.connect('edited',self.edit_macro_cell,column)
            treecol = Gtk.TreeViewColumn(title,renderer)
            treecol.set_expand(True)
            if column == 0:
                treecol.set_cell_data_func(renderer,lambda col, cell, model, it, data: cell.set_property('text',kind_names[model[it][0]]))
            elif column == 2:
                treecol.set_cell_data_func(renderer,lambda col, cell, model, it, data: cell.set_property('text','Press' if model[it][2] else 'Release'))
            else:
                treecol.add_attribute(renderer,'text',column)
            self.event_view.append_column(treecol)
        scroll = Gtk.ScrolledWindow()
        scroll.get_style_context().add_class('table')
        scroll.set_min_content_height(180)
        scroll.add(self.event_view)
        editor.pack_start(scroll, True, True, 0)
        row = Gtk.Box(spacing=8)
        self.event_kind = Gtk.ComboBoxText()
        for i,title in enumerate(kind_names):
            self.event_kind.append(str(i),title)
        self.event_kind.set_active(0)
        row.pack_start(self.event_kind, True, True, 0)
        self.event_code = Gtk.SpinButton.new_with_range(-128,255,1)
        self.event_code.set_value(4)
        self.event_code.set_tooltip_text('Keyboard: HID usage. Modifier: HID usage 224–231. Mouse: button mask 1–31. Movement: signed amount.')
        row.pack_start(self.event_code, False, False, 0)
        self.event_kind.connect('changed', self.event_kind_changed)
        self.event_pressed = Gtk.ComboBoxText()
        self.event_pressed.append('1','Press')
        self.event_pressed.append('0','Release')
        self.event_pressed.set_active(0)
        row.pack_start(self.event_pressed, False, False, 0)
        self.event_delay = Gtk.SpinButton.new_with_range(0,1048575,1)
        self.event_delay.set_value(50)
        self.event_delay.set_tooltip_text('Delay before this action, in milliseconds.')
        row.pack_start(self.event_delay, False, False, 0)
        row.pack_start(self.button('Add',self.add_macro_event), False, False, 0)
        editor.pack_start(row, False, False, 0)
        row = Gtk.Box(spacing=8)
        row.pack_start(self.button('Remove selected',self.remove_macro_event), False, False, 0)
        row.pack_start(self.button('Move up',lambda *_: self.move_macro_event(-1)), False, False, 0)
        row.pack_start(self.button('Move down',lambda *_: self.move_macro_event(1)), False, False, 0)
        editor.pack_start(row, False, False, 0)
        assign = self.card(page, 'Save and assign')
        self.macro_target = self.label('Choose a target key on the Keys page.', 'subtitle')
        assign.pack_start(self.macro_target, False, False, 0)
        self.macro_trigger = Gtk.ComboBoxText()
        for value,title in [('once','Play once'),('hold','While held'),('toggle','Toggle until another key')]:
            self.macro_trigger.append(value,title)
        self.macro_trigger.set_active(0)
        self.setting_row(assign,'Playback',None,self.macro_trigger)
        self.macro_repeat = Gtk.SpinButton.new_with_range(1,255,1)
        self.setting_row(assign,'Repeat count',None,self.macro_repeat)
        row = Gtk.Box(spacing=10)
        row.pack_start(self.button('Choose target key',lambda *_: self.navigate(None,'keys')),False,False,0)
        self.macro_save_button = self.button('Create macro and assign',self.save_macro,'primary')
        self.macro_update_button = self.button('Update opened macro',self.update_macro)
        self.macro_update_button.set_sensitive(False)
        row.pack_end(self.macro_update_button,False,False,0)
        row.pack_end(self.macro_save_button,False,False,0)
        assign.pack_start(row,False,False,0)
        self.macro_message = self.label('Creating a macro adds a new stored sequence and changes only the selected key.', 'subtitle')
        assign.pack_start(self.macro_message,False,False,0)

    def read_macros(self, *_):
        device = self.selected_device()
        if not device:
            return
        def complete(result):
            self.macros = result.get('macros',[])
            self.macro_combo.remove_all()
            for macro in self.macros:
                self.macro_combo.append(str(macro['index']),macro['name'])
            if self.macros:
                self.macro_combo.set_active(0)
            self.macro_usage.set_text('{} macros \u00b7 {} of {} bytes used'.format(len(self.macros),result.get('used',0),result.get('capacity',4096)))
            if self.key_action.get_active_id() == 'macro':
                self.key_action_changed()
            self.message.set_text('Stored macros loaded.')
        self.run_backend(['--macro-list',device['node']], 'Reading keyboard macros\u2026', complete)

    def open_macro(self, *_):
        selected = self.macro_combo.get_active_id()
        macro = next((m for m in self.macros if str(m['index']) == selected),None)
        if macro:
            self.edit_macro_index = macro['index']
            self.macro_update_button.set_sensitive(True)
            self.macro_name.set_text(macro['name'])
            self.macro_events.clear()
            for event in macro['events']:
                self.macro_events.append([event['kind'],event['code'],event['pressed'],event['delay_ms']])
            self.macro_message.set_text('Use Update opened macro to replace this sequence, or Create to save a separate copy.')

    def delete_macro(self, *_):
        device, selected = self.selected_device(), self.macro_combo.get_active_id()
        if not device or selected is None:
            return
        def complete(_result):
            self.macro_message.set_text('Macro deleted. Refresh the stored macro list to view the remaining sequences.')
            self.macros = []
            self.macro_combo.remove_all()
        self.run_backend(['--macro-delete',device['node'],'--index',selected], 'Removing the stored macro\u2026', complete)

    def add_macro_event(self, *_):
        kind, code = int(self.event_kind.get_active_id()),self.event_code.get_value_as_int()
        if not self.valid_macro_value(kind, code):
            self.macro_message.set_text('Use a key code 1–255, modifier 224–231, mouse mask 1–31, or nonzero movement −128–127.')
            return
        self.macro_events.append([kind,code,self.event_pressed.get_active_id()=='1',self.event_delay.get_value_as_int()])

    def edit_macro_cell(self, _renderer, path, text, column):
        try:
            value = int(text,0) if text.startswith('0x') else int(text)
            low, high = (-128,255) if column == 1 else (0,1048575)
            if low <= value <= high and (column != 1 or self.valid_macro_value(self.macro_events[path][0],value)):
                self.macro_events[path][column] = value
        except ValueError:
            self.macro_message.set_text('Enter a number within the event range.')

    def remove_macro_event(self, *_):
        model, selected = self.event_view.get_selection().get_selected()
        if selected:
            model.remove(selected)

    def move_macro_event(self, direction):
        model, selected = self.event_view.get_selection().get_selected()
        if selected:
            index = model.get_path(selected).get_indices()[0]
            target = index+direction
            if 0 <= target < len(model):
                model.swap(selected,model.get_iter(Gtk.TreePath.new_from_indices([target])))

    def toggle_recording(self, button):
        self.recording = button.get_active()
        self.last_record_event = time.monotonic()
        self.record_hint.set_text('Recording\u2026 keep this window focused. Press Escape to stop.' if self.recording else 'Recording stopped. Review the events before saving.')
        button.set_label('\u25a0  Stop recording' if self.recording else '\u25cf  Record keyboard')

    def record_key(self, _window, event, pressed):
        if not self.recording:
            return False
        name = Gdk.keyval_name(event.keyval) or ''
        if name == 'Escape':
            self.record_button.set_active(False)
            return True
        aliases = {'Return':'Enter','space':'Space','BackSpace':'Backspace','Control_L':'Left Ctrl','Control_R':'Right Ctrl',
                   'Shift_L':'Left Shift','Shift_R':'Right Shift','Alt_L':'Left Alt','Alt_R':'Right Alt',
                   'Super_L':'Left Super','Super_R':'Right Super','Caps_Lock':'Caps Lock','Page_Up':'Page Up','Page_Down':'Page Down',
                   'minus':'-','equal':'=','bracketleft':'[','bracketright':']','backslash':'\\','semicolon':';',
                   'apostrophe':"'",'grave':'`','comma':',','period':'.','slash':'/'}
        normalized = aliases.get(name,name.upper() if len(name)==1 else name)
        code = next((code for code,label in self.normal_keys() if label == normalized),None)
        if code is None:
            self.record_hint.set_text('That key is not mapped by the recorder. Add its HID code manually if needed.')
            return True
        now = time.monotonic()
        delay = min(1048575,round((now-self.last_record_event)*1000))
        self.last_record_event = now
        kind = 1 if 224 <= code <= 231 else 0
        self.macro_events.append([kind,code,pressed,delay])
        return True

    @staticmethod
    def valid_macro_value(kind, code):
        return (kind == 0 and 1 <= code <= 255) or (kind == 1 and 224 <= code <= 231) or (kind == 2 and 1 <= code <= 31) or (3 <= kind <= 5 and -128 <= code <= 127 and code != 0)

    def event_kind_changed(self, *_):
        kind = int(self.event_kind.get_active_id())
        low, high, value = (1,255,4) if kind == 0 else (224,231,224) if kind == 1 else (1,31,1) if kind == 2 else (-128,127,1)
        self.event_code.set_range(low,high)
        self.event_code.set_value(value)

    def macro_data(self):
        return {'name':self.macro_name.get_text().strip(),'events':[{'kind':r[0],'code':r[1],'pressed':r[2],'delay_ms':r[3]} for r in self.macro_events]}

    def save_macro(self, *_):
        device, data = self.selected_device(), self.macro_data()
        if not device or self.key_index is None:
            self.macro_message.set_text('Select a target key on the Keys page first.')
            return
        if not data['name'] or not data['events']:
            self.macro_message.set_text('Give the macro a name and add at least one event.')
            return
        with tempfile.NamedTemporaryFile(mode='w',prefix='aula-macro-',suffix='.csv',delete=False) as events:
            for event in data['events']:
                events.write('{kind},{code},{pressed},{delay_ms}\n'.format(**dict(event,pressed=int(event['pressed']))))
            path = events.name
        args = ['--macro-add',device['node'],'--name',data['name'],'--events',path,'--layer',self.layer_combo.get_active_id(),
                '--key',str(self.key_index),'--repeat',str(self.macro_repeat.get_value_as_int()),'--trigger',self.macro_trigger.get_active_id()]
        def complete(result):
            try:
                os.unlink(path)
            except OSError:
                pass
            self.macro_message.set_text('Macro {} saved and assigned to the selected key.'.format(result['index']))
            self.message.set_text('Macro created and assigned.')
        self.run_backend(args, 'Writing macro and key assignment\u2026', complete)

    def import_macro(self, *_):
        path = self.profile_file(False)
        if not path:
            return
        try:
            data = json.loads(Path(path).read_text())
            if not isinstance(data,dict) or not isinstance(data.get('events'),list):
                raise ValueError('Choose an AULA macro JSON file.')
            events = []
            for event in data['events']:
                kind,code,delay = int(event['kind']),int(event['code']),int(event['delay_ms'])
                if not self.valid_macro_value(kind,code) or not 0 <= delay <= 1048575:
                    raise ValueError('The macro contains an event outside the supported range.')
                events.append([kind,code,bool(event['pressed']),delay])
            self.macro_name.set_text(str(data.get('name',Path(path).stem)))
            self.edit_macro_index = None
            self.macro_update_button.set_sensitive(False)
            self.macro_events.clear()
            for event in events:
                self.macro_events.append(event)
            self.macro_message.set_text('Macro imported into the editor. Choose a target key before saving.')
        except (OSError,ValueError,KeyError,TypeError) as exc:
            self.macro_message.set_text('Could not import macro: '+str(exc))

    def export_macro(self, *_):
        path = self.profile_file(True)
        if path:
            try:
                Path(path).write_text(json.dumps(self.macro_data(),indent=2)+'\n')
                self.macro_message.set_text('Macro exported.')
            except OSError as exc:
                self.macro_message.set_text(str(exc))

    def update_macro(self, *_):
        device, data = self.selected_device(), self.macro_data()
        if not device or self.edit_macro_index is None:
            return
        if not data['name'] or not data['events']:
            self.macro_message.set_text('Give the macro a name and add at least one event.')
            return
        with tempfile.NamedTemporaryFile(mode='w',prefix='aula-macro-',suffix='.csv',delete=False) as events:
            for event in data['events']:
                events.write('{kind},{code},{pressed},{delay_ms}\n'.format(**dict(event,pressed=int(event['pressed']))))
            path = events.name
        def complete(_result):
            try:
                os.unlink(path)
            except OSError:
                pass
            self.macro_message.set_text('Macro updated. Its key assignments and all other macros were preserved.')
            self.message.set_text('Stored macro updated and verified.')
        self.run_backend(['--macro-update',device['node'],'--index',str(self.edit_macro_index),'--name',data['name'],'--events',path],
                         'Updating the stored macro…',complete)

    def build_studio(self):
        page = self.page('studio', 'Light in motion.', 'Live lighting runs on this computer while Studio is active.')
        card = self.card(page, 'Live effects')
        self.studio_pattern = Gtk.ComboBoxText()
        for value,label in [('wave','Color wave'),('breath','Breathing'),('rain','Falling stars'),('ripple','Expanding ripple'),('music','Music · audio level'),('image','Image / animated GIF')]:
            self.studio_pattern.append(value,label)
        self.studio_pattern.set_active(0)
        self.setting_row(card,'Pattern','Uses your selected color and effect palette.',self.studio_pattern)
        self.studio_speed = Gtk.Scale.new_with_range(Gtk.Orientation.HORIZONTAL,.2,3,.1)
        self.studio_speed.set_value(1)
        self.studio_speed.set_digits(1)
        self.studio_speed.set_value_pos(Gtk.PositionType.RIGHT)
        self.studio_speed.set_size_request(230,-1)
        self.setting_row(card,'Motion speed',None,self.studio_speed)
        self.studio_intensity = Gtk.Scale.new_with_range(Gtk.Orientation.HORIZONTAL,.05,1,.05)
        self.studio_intensity.set_value(.7)
        self.studio_intensity.set_digits(2)
        self.studio_intensity.set_value_pos(Gtk.PositionType.RIGHT)
        self.studio_intensity.set_size_request(230,-1)
        self.setting_row(card,'Intensity',None,self.studio_intensity)
        self.audio_target = Gtk.Entry()
        self.audio_target.set_text('auto')
        self.audio_target.set_placeholder_text('PipeWire sink node name, or auto')
        self.audio_target.set_width_chars(25)
        self.setting_row(card,'Playback source','Use auto for default playback, or enter a PipeWire sink name.',self.audio_target)
        self.studio_image_path = None
        self.studio_animation = None
        self.studio_image_button = self.button('Choose image / GIF…',self.choose_studio_image)
        self.setting_row(card,'Image animation','Pixel colors are mapped across the keyboard.',self.studio_image_button)
        self.studio_preview = KeyboardPreview(self)
        card.pack_start(self.studio_preview,False,False,0)
        actions = Gtk.Box(spacing=12)
        self.studio_start = self.button('Start live lighting',self.start_stream,'primary')
        self.studio_stop_button = self.button('Stop',self.stop_stream)
        actions.pack_start(self.studio_start,False,False,0)
        actions.pack_start(self.studio_stop_button,False,False,0)
        card.pack_start(actions,False,False,0)
        self.studio_message = self.label('Stopping restores the previous keyboard lighting. Music capture starts only when you click Start.', 'subtitle')
        card.pack_start(self.studio_message,False,False,0)

    def choose_studio_image(self, *_):
        dialog = Gtk.FileChooserDialog(title='Choose an image or animated GIF',transient_for=self,action=Gtk.FileChooserAction.OPEN)
        dialog.add_buttons('Cancel',Gtk.ResponseType.CANCEL,'Open',Gtk.ResponseType.OK)
        imagefilter = Gtk.FileFilter()
        imagefilter.set_name('Images')
        imagefilter.add_pixbuf_formats()
        dialog.add_filter(imagefilter)
        path = dialog.get_filename() if dialog.run()==Gtk.ResponseType.OK else None
        dialog.destroy()
        if path:
            try:
                self.studio_animation = GdkPixbuf.PixbufAnimation.new_from_file(path)
                self.studio_image_path = path
                self.studio_image_button.set_label(Path(path).name)
                self.studio_pattern.set_active_id('image')
            except GLib.Error as exc:
                self.studio_message.set_text('Could not open this image: '+str(exc))

    def studio_key_positions(self):
        keys = [k for k in KEY_LAYOUTS[self.model_combo.get_active_id() or 'F75'] if k[3]>k[1] and k[4]>k[2]]
        minx, miny, maxx, maxy = min(k[1] for k in keys),min(k[2] for k in keys),max(k[3] for k in keys),max(k[4] for k in keys)
        positions = [(0.,0.)]*126
        for _label,x1,y1,x2,y2,index in keys:
            if index<126:
                positions[index] = (((x1+x2)/2-minx)/(maxx-minx),((y1+y2)/2-miny)/(maxy-miny))
        return positions

    def start_stream(self, *_):
        if self.busy or self.streaming or not self.available() or not self.model_combo.get_active_id():
            return
        pattern = self.studio_pattern.get_active_id()
        if pattern == 'music' and not shutil.which('pw-record'):
            self.studio_message.set_text('Music mode needs PipeWire\u2019s pw-record command on this computer.')
            return
        if pattern == 'image' and self.studio_animation is None:
            self.studio_message.set_text('Choose an image or animated GIF first.')
            return
        self.streaming = True
        self.stream_stop = threading.Event()
        self.stream_after_stop = None
        self.stream_restore_colors = list(self.perkey_colors)
        self.stream_frames = None
        self.stream_music_level = 0.
        self.stream_audio = None
        self.stream_pattern = pattern
        self.stream_positions = self.studio_key_positions()
        self.stream_start_time = time.monotonic()
        self.stream_image_iter = self.studio_animation.get_iter(None) if pattern=='image' else None
        self.controls()
        self.studio_message.set_text('Live lighting is running. Stop restores your previous keyboard settings.')
        self.message.set_text('Studio is streaming live lighting to the keyboard.')
        node = self.selected_device()['node']
        audio_target = self.audio_target.get_text().strip()
        if audio_target == '@DEFAULT_AUDIO_SINK@':
            audio_target = 'auto'
        def worker():
            error = None
            process = None
            audio = None
            try:
                process = subprocess.Popen([self.binary,'--stream-stdin',node],stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
                if pattern == 'music':
                    command = ['pw-record','--raw','--rate','8000','--channels','1','--format','s16']
                    # Capture the output monitor of a sink instead of a microphone by default.
                    if audio_target == 'auto':
                        command += ['--properties','{ stream.capture.sink = true }']
                    else:
                        command += ['--target',audio_target,'--properties','{ stream.capture.sink = true }']
                    command += ['-']
                    audio = subprocess.Popen(command,stdout=subprocess.PIPE,stderr=subprocess.DEVNULL)
                    self.stream_audio = audio
                    def audio_reader():
                        while not self.stream_stop.is_set():
                            data = audio.stdout.read(800)
                            if not data:
                                break
                            samples = struct.unpack('<'+'h'*(len(data)//2),data[:len(data)//2*2])
                            level = math.sqrt(sum(v*v for v in samples)/max(1,len(samples)))/32768.
                            self.stream_music_level = min(1.,level*7)
                    threading.Thread(target=audio_reader,daemon=True).start()
                frames = 0
                while not self.stream_stop.is_set():
                    start = time.monotonic()
                    if process.poll() is not None:
                        raise RuntimeError(process.stderr.read().decode(errors='replace').strip() or 'The keyboard stream stopped unexpectedly.')
                    frame = self.stream_frames
                    if frame:
                        process.stdin.write(frame)
                        process.stdin.flush()
                        frames += 1
                    if audio is not None and audio.poll() is not None:
                        raise RuntimeError('PipeWire audio capture stopped. Check the music source and sound permissions.')
                    self.stream_stop.wait(max(0,.05-(time.monotonic()-start)))
            except (OSError,RuntimeError) as exc:
                error = str(exc)
            finally:
                if audio is not None:
                    audio.terminate()
                    try:
                        audio.wait(timeout=2)
                    except subprocess.TimeoutExpired:
                        audio.kill()
                if process is not None:
                    try:
                        process.stdin.close()
                        process.wait(timeout=5)
                        if process.returncode and not error:
                            error = process.stderr.read().decode(errors='replace').strip()
                    except (OSError,subprocess.TimeoutExpired):
                        process.terminate()
                        error = error or 'The stream could not confirm restoration. Read the keyboard before applying another setup.'
                GLib.idle_add(self.stream_finished,error)
        threading.Thread(target=worker,daemon=True).start()
        GLib.timeout_add(50,self.update_stream_frame)

    def update_stream_frame(self):
        if not self.streaming or self.stream_stop.is_set():
            return False
        pattern = self.stream_pattern
        elapsed = (time.monotonic()-self.stream_start_time)*self.studio_speed.get_value()
        intensity = self.studio_intensity.get_value()
        palette = [rgb(c) for c in self.palette]
        color = rgb(self.color)
        image = None
        if pattern=='image' and self.stream_image_iter:
            self.stream_image_iter.advance(None)
            image = self.stream_image_iter.get_pixbuf()
            pixels = image.get_pixels()
            stride, channels = image.get_rowstride(),image.get_n_channels()
        frame = bytearray()
        display = []
        for index,(x,y) in enumerate(self.stream_positions):
            if pattern=='wave':
                position = ((x-elapsed*.18)%1)*len(palette)
                left = int(position)
                blend = position-left
                a,b = palette[left%len(palette)],palette[(left+1)%len(palette)]
                values = tuple(a[i]*(1-blend)+b[i]*blend for i in range(3))
            elif pattern=='breath':
                strength = .06+.94*((math.sin(elapsed*2)+1)/2)**1.8
                values = tuple(v*strength for v in color)
            elif pattern=='rain':
                strength = max(0,math.sin(index*91.73+elapsed*2.8))**18
                values = tuple(v*(.03+.97*strength) for v in palette[index%len(palette)])
            elif pattern=='ripple':
                distance = math.sqrt((x-.5)**2+(y-.5)**2)*2
                strength = max(0,1-abs((elapsed*.4)%1.8-distance)*5)
                values = tuple(v*strength for v in color)
            elif pattern=='music':
                level = self.stream_music_level
                strength = min(1,max(.02,(level-y)*7))
                values = tuple(v*strength for v in palette[min(6,int(x*7))])
            elif image:
                ix,iy = min(image.get_width()-1,int(x*image.get_width())),min(image.get_height()-1,int(y*image.get_height()))
                offset = iy*stride+ix*channels
                values = tuple(pixels[offset+i]/255. for i in range(3))
            else:
                values = color
            values = [max(0,min(255,round(v*255*intensity))) for v in values]
            frame.extend(values)
            display.append('#{:02X}{:02X}{:02X}'.format(*values))
        self.stream_frames = bytes(frame)
        self.perkey_colors = display
        self.studio_preview.queue_draw()
        return True

    def stop_stream(self, *_args, after=None):
        if not self.streaming:
            if after:
                after()
            return
        if after:
            self.stream_after_stop = after
        self.stream_stop.set()
        self.studio_message.set_text('Stopping live lighting and restoring the previous keyboard setup\u2026')
        self.studio_stop_button.set_sensitive(False)

    def stream_finished(self, error):
        self.streaming = False
        self.perkey_colors = self.stream_restore_colors
        self.stream_stop = None
        self.stream_audio = None
        self.controls()
        self.studio_preview.queue_draw()
        self.keyboard_preview.queue_draw()
        self.studio_message.set_text(error or 'Live lighting stopped. The previous keyboard lighting was restored.')
        self.message.set_text(error or 'Studio stopped; previous keyboard settings restored.')
        callback,self.stream_after_stop = self.stream_after_stop,None
        if callback:
            callback()
        return False

    def on_destroy(self, *_):
        if self.preview_timer is not None:
            GLib.source_remove(self.preview_timer)
            self.preview_timer = None
        if self.stream_stop:
            self.stream_stop.set()
        # The native stream restores configuration when this process closes its input pipe.
        # Worker threads are daemons so they cannot keep a dismissed window alive.

    def autostart_changed(self, *_):
        try:
            if self.autostart.get_active():
                self.autostart_path.parent.mkdir(parents=True,exist_ok=True)
                executable = self.binary.replace('\\','\\\\').replace('"','\\"').replace('`','\\`').replace('$','\\$').replace('%','%%')
                self.autostart_path.write_text('[Desktop Entry]\nType=Application\nName=AULA Studio\nExec="'+executable+'" --gui\nTerminal=false\nX-GNOME-Autostart-enabled=true\n')
            else:
                self.autostart_path.unlink(missing_ok=True)
            self.message.set_text('Login preference saved on this computer.')
        except OSError as exc:
            self.message.set_text('Could not save the login preference: '+str(exc))

    def save_backup(self, *_):
        device = self.selected_device()
        if not device:
            return
        path = self.profile_file(True)
        if not path:
            return
        def complete(result):
            result['model'] = self.model_combo.get_active_id()
            result['format'] = 'aula-control-full-backup-v1'
            try:
                Path(path).write_text(json.dumps(result,indent=2)+'\n')
                self.message.set_text('Full keyboard backup saved.')
            except OSError as exc:
                self.message.set_text('Could not save the backup: '+str(exc))
        self.run_backend(['--snapshot',device['node']], 'Reading all keyboard settings for your backup\u2026', complete, timeout=45)

    def restore_backup(self, *_):
        device = self.selected_device()
        if not device:
            return
        path = self.profile_file(False)
        if not path:
            return
        try:
            data = json.loads(Path(path).read_text())
            fields = [('config_hex',256),('rgb_hex',1024),('macros_hex',8192),('perkey_hex',756)]
            for field,_length in fields:
                if not isinstance(data.get(field),str) or not re.fullmatch(r'[0-9a-fA-F]+',data[field]) or len(data[field]) != _length:
                    raise ValueError('The file is missing valid '+field+'.')
            maps = data.get('keymaps_hex')
            if not isinstance(maps,list) or len(maps)!=4 or any(not isinstance(m,str) or not re.fullmatch(r'[0-9a-fA-F]{1024}',m) for m in maps):
                raise ValueError('A full backup must include all four keyboard layers.')
            if data.get('model') and data['model'] != self.model_combo.get_active_id():
                raise ValueError('Choose the '+data['model']+' model before restoring this backup.')
        except (OSError,ValueError,TypeError,KeyError) as exc:
            self.message.set_text('Could not open the backup: '+str(exc))
            return
        dialog = Gtk.MessageDialog(transient_for=self,modal=True,message_type=Gtk.MessageType.QUESTION,
                                   buttons=Gtk.ButtonsType.CANCEL,text='Restore this full keyboard backup?')
        dialog.format_secondary_text('This replaces lighting, all key assignments, custom colors, and stored macros on the selected keyboard.')
        dialog.add_button('Restore backup',Gtk.ResponseType.OK)
        accepted = dialog.run() == Gtk.ResponseType.OK
        dialog.destroy()
        if not accepted:
            return
        args = ['--restore',device['node'],'--config',data['config_hex'],'--rgb',data['rgb_hex'],
                '--macros',data['macros_hex'],'--perkey',data['perkey_hex']]
        for index,value in enumerate(maps):
            args += ['--map'+str(index),value]
        def complete(_result):
            self.saved_snapshot = None
            self.known_fields.clear()
            self.touched_fields.clear()
            self.perkey_loaded = False
            self.message.set_text('Full backup restored and verified. Read keyboard to refresh the controls.')
        self.run_backend(args, 'Restoring and verifying your full keyboard backup\u2026', complete, timeout=60)

    def build_profiles(self):
        page = self.page('profiles', 'One setup for every mood.', 'Keep named lighting and device presets on this computer.')
        card = self.card(page, 'Your profiles')
        row = Gtk.Box(spacing=12)
        self.profile_combo = Gtk.ComboBoxText()
        self.profile_combo.set_hexpand(True)
        row.pack_start(self.profile_combo, True, True, 0)
        row.pack_start(self.button('Load', self.load_profile), False, False, 0)
        row.pack_start(self.button('Delete', self.delete_profile, 'destructive'), False, False, 0)
        card.pack_start(row, False, False, 0)
        row = Gtk.Box(spacing=12)
        self.profile_name = Gtk.Entry()
        self.profile_name.set_placeholder_text('Name this setup, e.g. Evening')
        row.pack_start(self.profile_name, True, True, 0)
        row.pack_start(self.button('Save current setup', self.save_profile, 'primary'), False, False, 0)
        card.pack_start(row, False, False, 0)
        card.pack_start(self.label('Loading a profile updates the controls. Apply changes to send it to the keyboard.', 'subtitle'), False, False, 0)
        share = self.card(page, 'Take your setup with you')
        buttons = Gtk.Box(spacing=12)
        buttons.pack_start(self.button('Import profile…', self.import_profile), False, False, 0)
        buttons.pack_start(self.button('Export current setup…', self.export_profile), False, False, 0)
        share.pack_start(buttons, False, False, 0)
        self.profile_message = self.label('Profiles store settings locally as readable JSON.', 'subtitle')
        share.pack_start(self.profile_message, False, False, 0)
        try:
            if self.profiles_path.exists():
                data = json.loads(self.profiles_path.read_text())
                if isinstance(data, dict):
                    self.profiles = data
        except (OSError, ValueError):
            self.profile_message.set_text('The saved profile file could not be read. Import a profile or save a new setup.')
        self.reload_profiles()

    def navigate(self, _button, name):
        if self.navigating:
            return
        self.navigating = True
        self.stack.set_visible_child_name(name)
        for key, button in self.nav_buttons.items():
            button.set_active(key == name)
        self.navigating = False

    def preview_positions(self):
        model = self.model_combo.get_active_id() or 'F75'
        keys = [k for k in KEY_LAYOUTS[model] if k[3] > k[1] and k[4] > k[2]]
        minx, miny = min(k[1] for k in keys), min(k[2] for k in keys)
        maxx, maxy = max(k[3] for k in keys), max(k[4] for k in keys)
        return [(index, ((x1+x2)/2-minx)/(maxx-minx), ((y1+y2)/2-miny)/(maxy-miny))
                for _label, x1, y1, x2, y2, index in keys]

    def preview_phase(self):
        return self.preview_elapsed

    def restart_preview(self):
        # Start breathing at its lit peak, rather than several seconds of black
        # at low speed. This is a presentation clock, never a device setting.
        self.preview_elapsed = 1.6 if self.effect_combo.get_active() == 2 else 0.0
        self.preview_presses = []
        self.preview_last_tick = time.monotonic()

    def update_preview_hint(self):
        if self.lighting_source.get_active_id() == 'perkey':
            text = 'Per-key preview · Choose a color, then paint keys or use Fill. Apply saves your pattern.'
        elif self.effect_combo.get_active() == 0:
            text = 'Preview is dark because the effect is Off. Choose an effect to preview lighting.'
        elif self.brightness.get_value() == 0:
            text = 'Preview is dark because brightness is zero. Increase brightness to see the effect.'
        elif self.color == '#000000' and not self.rainbow.get_active() and self.effect_combo.get_active() not in FIXED_COLOR_EFFECTS:
            text = 'The selected color is black, so the LEDs are off. Choose a brighter color to see the effect.'
        elif self.effect_combo.get_active() in REACTIVE_EFFECTS:
            text = 'Reactive preview · Demo keypresses play automatically. Click keys or type here to try it.'
        elif self.effect_combo.get_active() in FIXED_COLOR_EFFECTS:
            text = 'This effect uses firmware-defined rainbow colors. Choose Solid color to preview a custom color.'
        else:
            text = 'Animated preview only · Changes reach the keyboard when you click Apply. Firmware timing may vary.'
        if self.preview_paused:
            text = 'Preview paused · '+text
        self.preview_hint.set_text(text)

    def local_preview_frame(self):
        positions = self.preview_positions()
        phase = self.preview_phase()
        presses = [p for p in self.preview_presses if 0 <= phase-p[3] <= 3.5]
        if self.effect_combo.get_active() in REACTIVE_EFFECTS:
            # Automatic, explicitly local sample presses keep reactive modes
            # understandable before the user interacts with the keyboard image.
            cycle = int(phase/1.35)
            for previous in range(max(0, cycle-2), cycle+1):
                key, x, y = positions[(previous*23+17) % len(positions)]
                presses.append((key, x, y, previous*1.35))
        return lighting_effect_frame(self.effect_combo.get_active(), phase, positions,
                                     self.color, self.palette, self.rainbow.get_active(),
                                     self.brightness.get_value()/max(1, self.brightness.get_adjustment().get_upper()),
                                     presses)

    def preview_key_press(self, index):
        for key, x, y in self.preview_positions():
            if key == index:
                phase = self.preview_phase()
                self.preview_presses = [p for p in self.preview_presses if phase-p[3] < 3.5][-31:]
                self.preview_presses.append((key, x, y, phase))
                self.keyboard_preview.queue_draw()
                break

    def tick_preview(self):
        now = time.monotonic()
        delta = min(.1, max(0.0, now-self.preview_last_tick))
        self.preview_last_tick = now
        window = self.get_window()
        visible = self.get_mapped() and self.stack.get_visible_child_name() == 'lighting'
        if window and window.get_state() & Gdk.WindowState.ICONIFIED:
            visible = False
        if (visible and not self.preview_paused and not self.streaming
                and self.lighting_source.get_active_id() == 'effects'
                and self.effect_combo.get_active() in ANIMATED_EFFECTS):
            self.preview_elapsed += delta * (.35, .60, .90, 1.25, 1.65)[int(self.speed.get_value())]
            self.keyboard_preview.queue_draw()
        return True

    def toggle_preview(self, *_):
        self.preview_paused = not self.preview_paused
        self.preview_play.get_child().set_from_icon_name('media-playback-start-symbolic' if self.preview_paused else 'media-playback-pause-symbolic', Gtk.IconSize.BUTTON)
        self.preview_play.set_tooltip_text('Play preview' if self.preview_paused else 'Pause preview')
        self.preview_last_tick = time.monotonic()
        self.update_preview_hint()

    def selected_device(self):
        index = self.device_combo.get_active()
        return self.devices[index] if 0 <= index < len(self.devices) else None

    def available(self):
        d = self.selected_device()
        return bool(d and d.get('node_exists', True) and d.get('readable') and d.get('writable') and d.get('config_supported'))

    def controls(self):
        d = self.selected_device()
        available = self.available()
        model = bool(self.model_combo.get_active_id())
        self.refresh_button.set_sensitive(not self.busy)
        self.read_button.set_sensitive(not self.busy and available)
        self.apply_button.set_sensitive(not self.busy and available and model)
        self.key_read_button.set_sensitive(not self.busy and available and model)
        self.key_apply_button.set_sensitive(not self.busy and available and model and self.key_index is not None)
        self.macro_save_button.set_sensitive(not self.busy and available and model and self.key_index is not None)
        self.studio_start.set_sensitive(not self.busy and not self.streaming and available and model)
        self.studio_stop_button.set_sensitive(self.streaming and self.stream_stop is not None and not self.stream_stop.is_set())
        self.studio_pattern.set_sensitive(not self.streaming)
        self.lighting_source.set_sensitive(not self.busy and not self.streaming)
        blocked = bool(d and d.get('config_supported') and d.get('node_exists') and not available)
        pill = self.connection_pill.get_style_context()
        pill.remove_class('ok')
        pill.remove_class('warn')
        if available:
            pill.add_class('ok')
            self.connection_label.set_text('Connected')
        elif blocked:
            pill.add_class('warn')
            self.connection_label.set_text('Access blocked')
        else:
            self.connection_label.set_text('No keyboard')
        self.access_button.set_visible(blocked)
        self.access_button.set_sensitive(not self.busy and blocked)
        self.charging_light.set_sensitive(not self.busy and 'charging_light' in self.known_fields)
        self.apply_button.set_tooltip_text('Choose your keyboard model first.' if available and not model else
                                          'Enable wired keyboard access from Device settings.' if blocked else
                                          'Connect the keyboard by USB and select its control interface.' if not available else
                                          'Save the displayed settings to your keyboard.')
        for widget in (self.device_combo, self.model_combo, self.effect_combo, self.brightness, self.speed, self.rainbow,
                       self.color_button, self.color_entry, self.sleep_combo, self.debounce, self.key_action, self.key_target,
                       self.layer_combo):
            widget.set_sensitive(not self.busy)
        custom = self.lighting_source.get_active_id() == 'perkey'
        editable_color = not self.busy and (custom or self.effect_combo.get_active() not in FIXED_COLOR_EFFECTS)
        self.color_button.set_sensitive(editable_color)
        self.color_entry.set_sensitive(editable_color)
        self.quick_colors.set_sensitive(editable_color)
        self.palette_expander.set_sensitive(not self.busy and not custom and self.effect_combo.get_active() not in FIXED_COLOR_EFFECTS)
        if self.lighting_source.get_active_id() == 'perkey':
            self.color_button.set_sensitive(not self.busy)
            self.color_entry.set_sensitive(not self.busy)
            self.effect_combo.set_sensitive(False)
            self.brightness.set_sensitive(False)
            self.speed.set_sensitive(False)
            self.rainbow.set_sensitive(False)
        if self.effect_combo.get_active() in FIXED_COLOR_EFFECTS and not custom:
            self.color_button.set_sensitive(False)
            self.color_entry.set_sensitive(False)
            self.rainbow.set_sensitive(False)
        if not self.busy:
            self.update_save_status()

    def device_changed(self, *_):
        d = self.selected_device()
        node = d.get('node') if d else None
        if node != self.current_node:
            self.current_node = node
            self.known_fields.clear()
            self.touched_fields.clear()
            self.saved_snapshot = None
            self.perkey_loaded = False
        if not d:
            text = 'No AULA interface found. Connect by USB and refresh.'
        elif not d.get('node_exists', True):
            text = 'The USB interface is unavailable. Reconnect the keyboard.'
        elif not d.get('config_supported'):
            text = 'Wireless receiver detected. Connect the keyboard by USB for configuration.'
        elif not (d.get('readable') and d.get('writable')):
            text = 'Access blocked. Enable wired keyboard access below.'
        else:
            text = 'USB connected · '+d.get('node', '')
            if not self.model_combo.get_active_id():
                text += ' · Choose your model to enable Apply.'
            else:
                text += ' · Ready'
        self.device_status.set_text(text)
        self.controls()

    def model_changed(self, *_):
        if not hasattr(self, 'brightness'):
            return
        previous_loading = self.loading
        self.loading = True
        self.brightness.set_range(0, 9 if self.model_combo.get_active_id() == 'F75' else 4)
        self.loading = previous_loading
        self.preview_model.set_text('AULA '+(self.model_combo.get_active_id() or 'F SERIES'))
        self.key_index = None
        self.key_preview.selected = None
        self.key_label.set_text('Select a key above')
        self.changed(None, 'model')
        self.device_changed()

    def changed(self, _widget=None, field=None, *_):
        if self.loading:
            return
        if field:
            self.touched_fields.add(field)
        if field in ('mode', 'model', 'lighting_source', 'color', 'palette', 'rainbow'):
            self.restart_preview()
        if hasattr(self, 'preview_hint'):
            self.update_preview_hint()
        self.keyboard_preview.queue_draw()
        self.key_preview.queue_draw()
        if hasattr(self, 'studio_preview'):
            self.studio_preview.queue_draw()
        if field in ('mode', 'lighting_source') and hasattr(self, 'read_button'):
            self.controls()
        self.update_save_status()

    def rainbow_changed(self, *_):
        self.changed(None, 'rainbow')
        if hasattr(self, 'read_button'):
            self.controls()

    def device_switch_changed(self, _widget, _property, field):
        self.changed(None, field)

    def snapshot(self):
        return {'model': self.model_combo.get_active_id(), 'mode': self.effect_combo.get_active(),
                'brightness': int(self.brightness.get_value()), 'speed': int(self.speed.get_value()),
                'color': self.color, 'rainbow': self.rainbow.get_active(), 'palette': list(self.palette),
                'charging_light': self.charging_light.get_active() if 'charging_light' in self.known_fields else None,
                'sleep_seconds': int(self.sleep_combo.get_active_id() or '300'), 'debounce_ms': self.debounce.get_value_as_int(),
                'lighting_source': self.lighting_source.get_active_id(),
                'perkey_colors': list(self.perkey_colors) if self.perkey_loaded else None,
                'include_fields': sorted(self.known_fields | self.touched_fields)}

    def update_save_status(self):
        if self.saved_snapshot is None:
            text = 'Changes ready to apply' if self.touched_fields else 'Read your keyboard to get started'
        else:
            text = 'All changes saved' if self.snapshot() == self.saved_snapshot else 'Unsaved changes'
        self.save_status.set_text(text)

    def color_pixbuf(self, color, size=24):
        surface = cairo.ImageSurface(cairo.FORMAT_ARGB32, size, size)
        cr = cairo.Context(surface)
        cr.arc(size/2, size/2, size/2-.5, 0, 2*math.pi)
        cr.set_source_rgb(*rgb(color))
        cr.fill_preserve()
        cr.set_source_rgba(1, 1, 1, .18)
        cr.set_line_width(1)
        cr.stroke()
        stream = io.BytesIO()
        surface.write_to_png(stream)
        loader = GdkPixbuf.PixbufLoader.new_with_type('png')
        loader.write(stream.getvalue())
        loader.close()
        return loader.get_pixbuf()

    def swatch_button(self, color, callback):
        button = self.button('', callback, 'color')
        button.remove(button.get_child())
        area = Gtk.Image.new_from_pixbuf(self.color_pixbuf(color))
        button.swatch_image = area
        button.color = color
        button.add(area)
        button.set_tooltip_text(color)
        return button

    def set_color(self, color, touched=False):
        try:
            self.color = normalize_color(color)
        except ValueError as exc:
            self.message.set_text(str(exc))
            return
        self.updating_color_entry = True
        try:
            self.color_entry.set_text(self.color)
        finally:
            self.updating_color_entry = False
        self.color_swatch.set_from_pixbuf(self.color_pixbuf(self.color, 26))
        if touched:
            self.rainbow.set_active(False)
            self.changed(None, 'color')
        self.keyboard_preview.queue_draw()
        self.key_preview.queue_draw()

    def color_entered(self, *_):
        try:
            color = normalize_color(self.color_entry.get_text())
        except ValueError as exc:
            self.message.set_text(str(exc))
            return False
        # Merely leaving the field must not disable a multicolor effect or
        # restart its clock. Valid edits already commit through 'changed'.
        self.set_color(color, color != self.color)
        return False

    def color_text_changed(self, entry):
        if self.loading or self.updating_color_entry:
            return
        # Incomplete hex input is normal while typing. Commit valid RGB values
        # immediately without reformatting the entry or moving its cursor.
        try:
            color = normalize_color(entry.get_text())
        except ValueError:
            return
        self.color = color
        self.color_swatch.set_from_pixbuf(self.color_pixbuf(color, 26))
        self.rainbow.set_active(False)
        self.changed(None, 'color')

    @staticmethod
    def chooser_hex(dialog):
        c = dialog.get_rgba()
        return '#{:02X}{:02X}{:02X}'.format(round(c.red*255), round(c.green*255), round(c.blue*255))

    def pick_color(self, initial, title, preview=None):
        dialog = Gtk.ColorChooserDialog(title=title, transient_for=self)
        dialog.set_use_alpha(False)
        rgba = Gdk.RGBA()
        rgba.parse(initial)
        dialog.set_rgba(rgba)
        if preview:
            dialog.connect('notify::rgba', lambda chooser, _prop: preview(self.chooser_hex(chooser)))
        result = None
        if dialog.run() == Gtk.ResponseType.OK:
            result = self.chooser_hex(dialog)
        dialog.destroy()
        return result

    def choose_color(self, *_):
        previous, rainbow, touched = self.color, self.rainbow.get_active(), set(self.touched_fields)
        color = self.pick_color(self.color, 'Choose your lighting color', lambda c: self.set_color(c, True))
        self.set_color(color or previous, bool(color))
        if not color:
            self.rainbow.set_active(rainbow)
            self.touched_fields = touched
            self.changed()

    def choose_palette(self, index):
        previous, rainbow, touched = self.palette[index], self.rainbow.get_active(), set(self.touched_fields)
        def update(color):
            self.palette[index] = color
            self.palette_buttons[index].color = color
            self.palette_buttons[index].set_tooltip_text(color)
            self.palette_buttons[index].swatch_image.set_from_pixbuf(self.color_pixbuf(color))
            self.rainbow.set_active(True)
            self.changed(None, 'palette')
        color = self.pick_color(previous, 'Effect palette · color '+str(index+1), update)
        update(color or previous)
        if not color:
            self.rainbow.set_active(rainbow)
            self.touched_fields = touched
            self.changed()

    def run_backend(self, arguments, message, callback, timeout=20):
        if self.streaming:
            self.stop_stream(after=lambda: self.run_backend(arguments, message, callback, timeout))
            return
        if self.busy:
            return
        self.busy = True
        self.controls()
        self.save_status.set_text('Working…')
        self.message.set_text(message)
        def worker():
            result, error = None, None
            try:
                process = subprocess.run([self.binary]+arguments, capture_output=True, text=True, timeout=timeout, check=False)
                if process.returncode:
                    error = (process.stderr or process.stdout).strip() or 'The device operation failed.'
                else:
                    result = json.loads(process.stdout)
            except subprocess.TimeoutExpired:
                error = 'The keyboard did not respond in time. Reconnect it by USB and refresh.'
            except (OSError, ValueError) as exc:
                error = str(exc)
            GLib.idle_add(finish, result, error)
        def finish(result, error):
            self.busy = False
            if error:
                self.output.get_buffer().set_text(error)
                # Preserve useful backend messages, keeping long payloads out of the main UI.
                clean = error.splitlines()[0]
                self.message.set_text(clean[:240])
                self.save_status.set_text('Could not complete the operation')
                self.controls()
                self.save_status.set_text('Could not complete the operation')
            else:
                self.output.get_buffer().set_text(json.dumps(result, indent=2, ensure_ascii=False))
                try:
                    callback(result)
                except (ValueError, KeyError, TypeError) as exc:
                    self.message.set_text('The keyboard returned an unexpected configuration. See Device diagnostics.')
                    self.output.get_buffer().set_text(str(exc)+'\n'+json.dumps(result, indent=2))
                self.controls()
            return False
        threading.Thread(target=worker, daemon=True).start()

    def refresh(self, *_):
        previous = (self.selected_device() or {}).get('node')
        def complete(result):
            self.devices = result if isinstance(result, list) else []
            self.device_combo.remove_all()
            selected = next((i for i,d in enumerate(self.devices) if d.get('config_supported') and d.get('readable') and d.get('writable')), next((i for i,d in enumerate(self.devices) if d.get('config_supported')), 0))
            for index, d in enumerate(self.devices):
                title = d.get('name', 'AULA keyboard')
                if len(title) > 36:
                    title = title[:33]+'…'
                self.device_combo.append_text(title+' · '+('USB' if d.get('config_supported') else 'Receiver'))
                if d.get('node') == previous and d.get('config_supported'):
                    selected = index
            self.device_combo.set_active(selected if self.devices else -1)
            self.device_changed()
            self.message.set_text('Connected. Read keyboard to load its current settings.' if self.available() else self.device_status.get_text())
        self.run_backend(['--list'], 'Looking for your keyboard…', complete)
        return False

    def setup_access(self, *_):
        self.run_backend(['--setup-permissions'], 'Authenticate in the system dialog to enable keyboard access…', lambda _r: self.refresh(), timeout=90)

    def populate(self, result, from_device=False):
        self.loading = True
        try:
            model = result.get('model')
            if model in KEY_LAYOUTS:
                self.model_combo.set_active_id(model)
            if isinstance(result.get('mode'), int) and 0 <= result['mode'] < len(EFFECTS):
                self.effect_combo.set_active(result['mode'])
                if from_device:
                    self.lighting_source.set_active_id('effects')
            elif result.get('mode') == 21 and from_device:
                self.lighting_source.set_active_id('perkey')
            for field, widget in [('brightness',self.brightness), ('speed',self.speed), ('debounce_ms',self.debounce)]:
                if isinstance(result.get(field), (int,float)) and result[field] >= 0:
                    widget.set_value(result[field])
                    self.known_fields.add(field)
            if result.get('color'):
                self.set_color(result['color'])
                self.known_fields.add('color')
            if isinstance(result.get('palette'), list) and len(result['palette']) == 7:
                self.palette = [normalize_color(c) for c in result['palette']]
                for i, button in enumerate(self.palette_buttons):
                    button.color = self.palette[i]
                    button.swatch_image.set_from_pixbuf(self.color_pixbuf(self.palette[i]))
                self.known_fields.add('palette')
            if isinstance(result.get('rainbow'), bool):
                self.rainbow.set_active(result['rainbow'])
                self.known_fields.add('rainbow')
            if isinstance(result.get('charging_light'), bool):
                self.charging_light.set_active(result['charging_light'])
                self.known_fields.add('charging_light')
                self.charging_hint.set_text('Supported by the connected keyboard.')
            elif from_device:
                self.known_fields.discard('charging_light')
                self.charging_hint.set_text('This firmware does not expose a confirmed charging-light control yet.')
            if result.get('sleep_seconds') in SLEEP_TIMES:
                self.sleep_combo.set_active_id(str(result['sleep_seconds']))
                self.known_fields.add('sleep_seconds')
            if result.get('lighting_source') in ('effects', 'perkey'):
                self.lighting_source.set_active_id(result['lighting_source'])
            if isinstance(result.get('perkey_colors'), list) and len(result['perkey_colors']) == 126:
                self.perkey_colors = [normalize_color(c) for c in result['perkey_colors']]
                self.perkey_loaded = True
        finally:
            self.loading = False
        self.restart_preview()
        self.update_preview_hint()
        self.controls()
        self.keyboard_preview.queue_draw()
        self.key_preview.queue_draw()

    def read_device(self, *_):
        device = self.selected_device()
        if not device:
            return
        def complete(result):
            self.known_fields.clear()
            self.touched_fields.clear()
            self.populate(result, True)
            self.saved_snapshot = self.snapshot()
            self.message.set_text('Current settings loaded. Changes are saved when you click Apply changes.')
        self.run_backend(['--read',device['node']], 'Reading your keyboard…', complete)

    def settings_arguments(self, device_only=False):
        settings = self.snapshot()
        args = ['--model',settings['model']]
        changed = set(self.touched_fields)
        lighting = not device_only and settings['lighting_source'] == 'effects'
        if lighting:
            if 'mode' in changed or 'lighting_source' in changed or self.saved_snapshot is None:
                args += ['--mode',str(settings['mode'])]
            for field in ('brightness','speed'):
                if field in changed or self.saved_snapshot is None:
                    args += ['--'+field,str(settings[field])]
        for field, flag in [('color','--color'), ('rainbow','--rainbow'), ('charging_light','--charging-light'),
                            ('sleep_seconds','--sleep'), ('debounce_ms','--debounce')]:
            if field not in changed or settings[field] is None:
                continue
            if field in ('color','rainbow') and (not lighting or settings['mode'] in (0,3,15,16,17)):
                continue
            value = settings[field]
            if isinstance(value,bool):
                value = int(value)
            elif field == 'color':
                value = value.lstrip('#')
            args += [flag,str(value)]
        if lighting and 'palette' in changed and settings['mode'] not in (0,3,15,16,17):
            args += ['--palette',','.join(c.lstrip('#') for c in self.palette)]
        return args

    def apply(self, *_):
        device = self.selected_device()
        if not device:
            return
        def complete(result):
            if isinstance(result,dict):
                values = result.get('configuration',result)
                if isinstance(values,dict):
                    self.populate(values,True)
            self.touched_fields.clear()
            self.saved_snapshot = self.snapshot()
            self.message.set_text('Your settings were applied to the keyboard.')
        if self.lighting_source.get_active_id() == 'perkey' and self.touched_fields.intersection({'perkey_colors','lighting_source'}):
            if not self.perkey_loaded:
                self.message.set_text('Read key colors or use Fill before applying a custom color pattern.')
                return
            extra = self.settings_arguments(device_only=True)
            def painted(result):
                if len(extra)>2:
                    self.run_backend(['--apply',device['node']]+extra,'Saving device behavior\u2026',complete)
                else:
                    complete(result)
            self.run_backend(['--paint',device['node'],'--model',self.model_combo.get_active_id(),
                              '--colors',''.join(c.lstrip('#') for c in self.perkey_colors)],
                             'Saving your individual key colors\u2026',painted)
            return
        args = self.settings_arguments()
        if len(args)==2:
            self.message.set_text('No settings have changed. Choose an option to edit first.')
            return
        self.run_backend(['--apply',device['node']]+args,'Saving your settings\u2026',complete)

    def key_action_changed(self, *_):
        if not hasattr(self, 'key_target'):
            return
        self.key_target.remove_all()
        action = self.key_action.get_active_id()
        if action == 'media':
            choices = [(0xE2,'Mute'),(0xE9,'Volume up'),(0xEA,'Volume down'),(0xCD,'Play / pause'),(0xB5,'Next track'),(0xB6,'Previous track'),(0xB7,'Stop'),(0x183,'Media player'),(0x18A,'Email'),(0x192,'Calculator'),(0x221,'Search'),(0x223,'Browser home'),(0x224,'Browser back'),(0x225,'Browser forward'),(0x226,'Browser stop'),(0x227,'Browser refresh'),(0x22A,'Browser bookmarks')]
        elif action == 'mouse':
            choices = [(0x11,'Mouse button 1'),(0x12,'Mouse button 3'),(0x13,'Mouse button 2'),(0x14,'Mouse button 4'),(0x15,'Mouse button 5'),(0x16,'Mouse double click'),(0x17,'Mouse triple click'),(0x19,'Vendor mouse action 0x19'),(0x1A,'Vendor mouse action 0x1A'),(0x1B,'Vendor mouse action 0x1B'),(0x1C,'Vendor mouse action 0x1C')]
        elif action == 'macro':
            choices = [(macro['index'],macro['name']) for macro in self.macros]
            if not choices:
                choices = [(-1,'Read or create macros first')]
        elif action == 'special':
            codes = [0x07000000,0x07000001,0x07000002,0x07000004,0x07000005,0x07000006,0x07000007,0x07000008,0x07000009,0x0700000a,0x0700000c,0x07000011,0x07000018,0x07000019,0x0700001a,0x0700001b,0x08000000,0x08000001,0x08000002,0x08020000,0x08020001,0x08020002,0x08030001,0x08030100,0x08030200,0x08040001,0x08040002,0x08040100,0x08040200,0x10000002,0x10000004,0x05000007,0x06000007,0x08000007,0x09000007,0x0d000000,0x0d010000,0x05010000,0x05020000,0x05040000,0x0e000000]
            names = {0x07000001:'Windows key lock',0x07000004:'Original Fn + Esc',0x08000000:'Original Fn + backslash',0x08000001:'Original Fn + left Shift',0x08020000:'Original Fn + Tab',0x08030100:'Original Fn + Up',0x08030200:'Original Fn + Down',0x08040100:'Original Fn + Right',0x08040200:'Original Fn + Left',0x07000005:'Original Fn + 1',0x07000006:'Original Fn + 2',0x07000007:'Original Fn + 3',0x07000008:'Original Fn + grave',0x0700000A:'Original Fn + G',0x07000011:'Original Fn + B',0x07000019:'Original Fn + Q',0x0700001A:'Original Fn + W',0x07000018:'Original Fn + E'}
            choices = [(code,names.get(code,'Firmware action 0x{:08X}'.format(code))) for code in codes]
        else:
            choices = self.normal_keys()
        for code, label in choices:
            self.key_target.append(str(code),label)
        self.key_target.set_active(0)
        self.key_target_row.set_visible(action != 'disable')
        self.modifier_row.set_visible(action == 'combo')
        if hasattr(self,'code2_row'):
            self.code2_row.set_visible(action == 'combo')

    def select_key(self, label, index):
        self.key_index = index
        self.key_label.set_text(label+' · matrix position '+str(index))
        self.key_message.set_text('Choose a replacement action, then save this key assignment.')
        self.macro_target.set_text('Target: '+label+' · '+self.layer_combo.get_active_text())
        record = next((key for key in self.keymap if key.get('index') == index), None)
        if record:
            action = record.get('type')
            if action in ('key','combo','media','mouse','special','macro','disable'):
                self.key_action.set_active_id(action)
                if action != 'disable':
                    if not self.key_target.set_active_id(str(record.get('code'))):
                        self.key_target.append(str(record.get('code')), 'Current action · '+str(record.get('code')))
                        self.key_target.set_active_id(str(record.get('code')))
                for button, bit in self.modifiers:
                    button.set_active(bool(record.get('modifiers',0) & bit))
                self.code2_combo.set_active_id(str(record.get('code2',0)))
                self.key_message.set_text('Current assignment loaded. Change the action and save to update this key.')
        self.controls()

    def read_keymap(self, *_):
        device = self.selected_device()
        if not device:
            return
        def complete(result):
            self.keymap = result if isinstance(result,list) else result.get('keys', [])
            self.key_message.set_text('Read '+str(len(self.keymap))+' assignments from '+self.layer_combo.get_active_text()+'.')
            self.message.set_text('Key assignments loaded.')
        self.run_backend(['--keymap',device['node'],'--layer',self.layer_combo.get_active_id()], 'Reading key assignments…', complete)

    def apply_key(self, *_):
        device = self.selected_device()
        if not device or self.key_index is None:
            return
        args = ['--remap',device['node'],'--layer',self.layer_combo.get_active_id(),'--key',str(self.key_index),'--action',self.key_action.get_active_id()]
        if self.key_action.get_active_id() != 'disable':
            args += ['--code',self.key_target.get_active_id()]
        if self.key_action.get_active_id() == 'combo':
            args += ['--modifiers',str(sum(bit for button,bit in self.modifiers if button.get_active()))]
            args += ['--code2',self.code2_combo.get_active_id() or '0']
        if self.key_action.get_active_id() == 'macro':
            if self.key_target.get_active_id() in (None,'-1'):
                self.key_message.set_text('Read or create stored macros on the Macros page first.')
                return
            args += ['--repeat',str(self.macro_repeat.get_value_as_int()),'--trigger',self.macro_trigger.get_active_id()]
        def complete(_result):
            self.key_message.set_text('Assignment saved to the keyboard.')
            self.message.set_text('Key assignment applied.')
        self.run_backend(args, 'Saving key assignment…', complete)

    def reload_profiles(self, selected=None):
        self.profile_combo.remove_all()
        for name in sorted(self.profiles):
            self.profile_combo.append(name,name)
        if selected:
            self.profile_combo.set_active_id(selected)
        elif self.profiles:
            self.profile_combo.set_active(0)

    def persist_profiles(self):
        self.profiles_path.parent.mkdir(parents=True, exist_ok=True)
        temporary = self.profiles_path.with_suffix('.tmp')
        temporary.write_text(json.dumps(self.profiles, indent=2)+'\n')
        temporary.replace(self.profiles_path)

    def save_profile(self, *_):
        name = self.profile_name.get_text().strip()
        if not name:
            self.profile_message.set_text('Enter a name for your setup first.')
            return
        self.profiles[name] = self.snapshot()
        try:
            self.persist_profiles()
        except OSError as exc:
            self.profile_message.set_text('Could not save profiles: '+str(exc))
            return
        self.reload_profiles(name)
        self.profile_message.set_text('Saved “'+name+'” on this computer.')

    def validate_profile(self, data):
        if not isinstance(data,dict) or data.get('model') not in KEY_LAYOUTS:
            raise ValueError('The profile must specify an AULA F75, F87, or F99 model.')
        if not isinstance(data.get('mode'),int) or not 0 <= data['mode'] < len(EFFECTS):
            raise ValueError('The profile contains an invalid lighting effect.')
        for field, maximum in [('brightness',9 if data['model']=='F75' else 4),('speed',4),('debounce_ms',5)]:
            if field in data and (not isinstance(data[field],int) or not 0 <= data[field] <= maximum):
                raise ValueError('The profile contains an invalid '+field+'.')
        if data.get('color'):
            normalize_color(data['color'])
        if 'sleep_seconds' in data and data['sleep_seconds'] not in SLEEP_TIMES:
            raise ValueError('The profile contains an unsupported sleep timer.')
        if 'palette' in data and (not isinstance(data['palette'],list) or len(data['palette'])!=7):
            raise ValueError('An effect palette must contain seven colors.')
        for color in data.get('palette',[]):
            normalize_color(color)
        return data

    def use_profile(self, data):
        data = self.validate_profile(data)
        self.populate(data)
        self.touched_fields.update(data.get('include_fields', [k for k in data if k not in ('include_fields','model')]))
        self.controls()
        self.profile_message.set_text('Profile loaded into the controls. Apply changes when you are ready.')
        self.message.set_text('Profile loaded. Apply changes to save it to your keyboard.')

    def load_profile(self, *_):
        name = self.profile_combo.get_active_id()
        if name:
            try:
                self.use_profile(self.profiles[name])
            except (ValueError,TypeError) as exc:
                self.profile_message.set_text(str(exc))

    def delete_profile(self, *_):
        name = self.profile_combo.get_active_id()
        if name:
            del self.profiles[name]
            try:
                self.persist_profiles()
                self.reload_profiles()
                self.profile_message.set_text('Deleted “'+name+'”.')
            except OSError as exc:
                self.profile_message.set_text(str(exc))

    def profile_file(self, save):
        dialog = Gtk.FileChooserDialog(title='Export lighting profile' if save else 'Import lighting profile', transient_for=self,
                                       action=Gtk.FileChooserAction.SAVE if save else Gtk.FileChooserAction.OPEN)
        dialog.add_buttons('Cancel',Gtk.ResponseType.CANCEL,'Export' if save else 'Import',Gtk.ResponseType.OK)
        filefilter = Gtk.FileFilter()
        filefilter.set_name('AULA profiles (*.json)')
        filefilter.add_pattern('*.json')
        dialog.add_filter(filefilter)
        if save:
            dialog.set_do_overwrite_confirmation(True)
            dialog.set_current_name('aula-profile.json')
        path = dialog.get_filename() if dialog.run() == Gtk.ResponseType.OK else None
        dialog.destroy()
        return path

    def export_profile(self, *_):
        path = self.profile_file(True)
        if path:
            try:
                Path(path).write_text(json.dumps(self.snapshot(),indent=2)+'\n')
                self.profile_message.set_text('Profile exported.')
            except OSError as exc:
                self.profile_message.set_text(str(exc))

    def import_profile(self, *_):
        path = self.profile_file(False)
        if path:
            try:
                self.use_profile(json.loads(Path(path).read_text()))
                self.profile_name.set_text(Path(path).stem)
            except (OSError,ValueError,TypeError) as exc:
                self.profile_message.set_text('Could not import this profile: '+str(exc))


def main():
    if len(sys.argv) < 2:
        raise SystemExit('Launch this interface with the aula-control executable.')
    if not Gtk.init_check()[0]:
        raise SystemExit('A graphical display is required. Use aula-control --help for command-line options.')
    ControlWindow(sys.argv[1])
    Gtk.main()


if __name__ == '__main__':
    main()
