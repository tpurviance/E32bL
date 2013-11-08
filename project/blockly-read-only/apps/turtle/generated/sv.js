// This file was automatically generated from common.soy.
// Please don't edit this file by hand.

if (typeof apps == 'undefined') { var apps = {}; }


apps.messages = function(opt_data, opt_ignored, opt_ijData) {
  return '<div style="display: none"><span id="subtitle">en visuell programmeringsmiljö</span><span id="blocklyMessage">Blockly</span><span id="codeTooltip">Se genererad JavaScript-kod.</span><span id="linkTooltip">Spara och länka till block.</span><span id="runTooltip">Kör programmet definierat av blocken i arbetsytan.</span><span id="runProgram">Kör program</span><span id="resetProgram">Återställ</span><span id="dialogOk">OK</span><span id="dialogCancel">Avbryt</span><span id="catLogic">Logisk</span><span id="catLoops">Loopar</span><span id="catMath">Matematik</span><span id="catText">Text</span><span id="catLists">Listor</span><span id="catColour">Färg</span><span id="catVariables">Variabler</span><span id="catProcedures">Procedurer</span><span id="httpRequestError">Det uppstod ett problem med begäran.</span><span id="linkAlert">Dela dina block med denna länk: \n\n%1</span><span id="hashError">Tyvärr, \'%1\' överensstämmer inte med något sparat program.</span><span id="xmlError">Kunde inte läsa din sparade fil. Den skapades kanske med en annan version av Blockly?</span><span id="listVariable">lista</span><span id="textVariable">text</span></div>';
};


apps.dialog = function(opt_data, opt_ignored, opt_ijData) {
  return '<div id="dialogShadow" class="dialogAnimate"></div><div id="dialogBorder"></div><div id="dialog"></div>';
};


apps.codeDialog = function(opt_data, opt_ignored, opt_ijData) {
  return '<div id="dialogCode" class="dialogHiddenContent"><pre id="containerCode"></pre>' + apps.ok(null, null, opt_ijData) + '</div>';
};


apps.storageDialog = function(opt_data, opt_ignored, opt_ijData) {
  return '<div id="dialogStorage" class="dialogHiddenContent"><div id="containerStorage"></div>' + apps.ok(null, null, opt_ijData) + '</div>';
};


apps.ok = function(opt_data, opt_ignored, opt_ijData) {
  return '<div class="farSide" style="padding: 1ex 3ex 0"><button class="secondary" onclick="BlocklyApps.hideDialog(true)">OK</button></div>';
};

;
// This file was automatically generated from template.soy.
// Please don't edit this file by hand.

if (typeof turtlepage == 'undefined') { var turtlepage = {}; }


turtlepage.messages = function(opt_data, opt_ignored, opt_ijData) {
  return apps.messages(null, null, opt_ijData) + '<div style="display: none"><span id="Turtle_moveTooltip">Flyttar sköldpaddan framåt eller bakåt enligt \\nden angivna summan. </span><span id="Turtle_moveForward">flytta framåt</span><span id="Turtle_moveBackward">flytta bakåt</span><span id="Turtle_turnTooltip">Vrider sköldpaddan vänster eller höger enligt \\ndet angivna antalet grader. </span><span id="Turtle_turnRight">sväng höger</span><span id="Turtle_turnLeft">sväng vänster</span><span id="Turtle_widthTooltip">Ändrar bredden på pennan.</span><span id="Turtle_setWidth">anger bredd till</span><span id="Turtle_colourTooltip">Ändrar färgen på pennan.</span><span id="Turtle_setColour">anger färg till</span><span id="Turtle_penTooltip">Lyfter eller sänker pennan, för att sluta eller \\nbörja rita. </span><span id="Turtle_penUp">penna upp</span><span id="Turtle_penDown">penna ned</span><span id="Turtle_turtleVisibilityTooltip">Gör sköldpaddan (cirkel och pil) synlig eller \\nosynlig. </span><span id="Turtle_hideTurtle">dölj sköldpadda</span><span id="Turtle_showTurtle">visa sköldpadda</span><span id="Turtle_printHelpUrl">http://sv.wikipedia.org/wiki/Tryckteknik</span><span id="Turtle_printTooltip">Ritar text i sköldpaddans riktning på dess plats.</span><span id="Turtle_print">visa</span><span id="Turtle_fontHelpUrl">http://sv.wikipedia.org/wiki/Vikt_%28typografi%29</span><span id="Turtle_fontTooltip">Anger typsnitten som används av textblocket.</span><span id="Turtle_font">typsnitt</span><span id="Turtle_fontSize">teckenstorlek</span><span id="Turtle_fontNormal">normal</span><span id="Turtle_fontBold">fet</span><span id="Turtle_fontItalic">kursiv</span><span id="Turtle_unloadWarning">Om du lämnar denna sida kommer ditt arbete förloras.</span></div>';
};


turtlepage.start = function(opt_data, opt_ignored, opt_ijData) {
  return turtlepage.messages(null, null, opt_ijData) + '<table width="100%"><tr><td><h1><span id="title"><a href="../index.html">Blockly</a> : Sköldpaddans grafik</span></h1></td><td class="farSide"><select id="languageMenu"></select></td></tr></table><div id="visualization"><canvas id="scratch" width="400" height="400" style="display: none"></canvas><canvas id="display" width="400" height="400"></canvas></div><table style="padding-top: 1em;"><tr><td style="width: 190px; text-align: center; vertical-align: top;"><script type="text/javascript" src="../slider.js"><\/script><svg id="slider" xmlns="http://www.w3.org/2000/svg" xmlns:svg="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" version="1.1" width="150" height="50"><!-- Slow icon. --><clipPath id="slowClipPath"><rect width=26 height=12 x=5 y=14 /></clipPath><image xlink:href="icons.png" height=42 width=84 x=-21 y=-10 clip-path="url(#slowClipPath)" /><!-- Fast icon. --><clipPath id="fastClipPath"><rect width=26 height=16 x=120 y=10 /></clipPath><image xlink:href="icons.png" height=42 width=84 x=120 y=-11 clip-path="url(#fastClipPath)" /></svg></td><td style="width: 15px;"><img id="spinner" style="visibility: hidden;" src="loading.gif" height=15 width=15></td><td style="width: 190px; text-align: center"><button id="runButton" class="primary" title="Får sköldpaddan att göra vad blocken säger."><img src="../../media/1x1.gif" class="run icon21">Kör program</button><button id="resetButton" class="primary" style="display: none"><img src="../../media/1x1.gif" class="stop icon21"> Återställ</button></td></tr></table><div id="toolbarDiv"><button id="codeButton" class="notext" title="Se genererad JavaScript-kod."><img src=\'../../media/1x1.gif\' class="code icon21"></button><button id="linkButton" class="notext" title="Spara och länka till block."><img src=\'../../media/1x1.gif\' class="link icon21"></button><button class="notext" id="captureButton" title="Spara ritningen."><img src=\'../../media/1x1.gif\' class="img icon21"></button><a id="downloadImageLink" download="ritning.png"></a></div><script type="text/javascript" src="../../blockly_compressed.js"><\/script><script type="text/javascript" src="../../blocks_compressed.js"><\/script><script type="text/javascript" src="../../javascript_compressed.js"><\/script><script type="text/javascript" src="../../' + soy.$$escapeHtml(opt_ijData.langSrc) + '"><\/script><script type="text/javascript" src="blocks.js"><\/script>' + turtlepage.toolbox(null, null, opt_ijData) + '<div id="blockly"></div>' + apps.dialog(null, null, opt_ijData) + apps.codeDialog(null, null, opt_ijData) + apps.storageDialog(null, null, opt_ijData);
};


turtlepage.toolbox = function(opt_data, opt_ignored, opt_ijData) {
  return '<xml id="toolbox" style="display: none"><category name="Sköldpadda"><block type="draw_move"><value name="VALUE"><block type="math_number"><title name="NUM">10</title></block></value></block><block type="draw_turn"><value name="VALUE"><block type="math_number"><title name="NUM">90</title></block></value></block><block type="draw_width"><value name="WIDTH"><block type="math_number"><title name="NUM">1</title></block></value></block><block type="draw_pen"></block><block type="turtle_visibility"></block><block type="draw_print"><value name="TEXT"><block type="text"></block></value></block><block type="draw_font"></block></category><category name="Färg"><block type="draw_colour"><value name="COLOUR"><block type="colour_picker"></block></value></block><block type="colour_picker"></block><block type="colour_random"></block><block type="colour_rgb"></block><block type="colour_blend"></block></category><category name="Logisk"><block type="controls_if"></block><block type="logic_compare"></block><block type="logic_operation"></block><block type="logic_negate"></block><block type="logic_boolean"></block><block type="logic_ternary"></block></category><category name="Loopar"><block type="controls_repeat_ext"><value name="TIMES"><block type="math_number"><title name="NUM">10</title></block></value></block><block type="controls_whileUntil"></block><block type="controls_for"><value name="FROM"><block type="math_number"><title name="NUM">1</title></block></value><value name="TO"><block type="math_number"><title name="NUM">10</title></block></value><value name="BY"><block type="math_number"><title name="NUM">1</title></block></value></block><block type="controls_forEach"></block><block type="controls_flow_statements"></block></category><category name="Matematik"><block type="math_number"></block><block type="math_arithmetic"></block><block type="math_single"></block><block type="math_trig"></block><block type="math_constant"></block><block type="math_number_property"></block><block type="math_change"><value name="DELTA"><block type="math_number"><title name="NUM">1</title></block></value></block><block type="math_round"></block><block type="math_on_list"></block><block type="math_modulo"></block><block type="math_constrain"><value name="LOW"><block type="math_number"><title name="NUM">1</title></block></value><value name="HIGH"><block type="math_number"><title name="NUM">100</title></block></value></block><block type="math_random_int"><value name="FROM"><block type="math_number"><title name="NUM">1</title></block></value><value name="TO"><block type="math_number"><title name="NUM">100</title></block></value></block><block type="math_random_float"></block></category><category name="Listor"><block type="lists_create_empty"></block><block type="lists_create_with"></block><block type="lists_repeat"><value name="NUM"><block type="math_number"><title name="NUM">5</title></block></value></block><block type="lists_length"></block><block type="lists_isEmpty"></block><block type="lists_indexOf"><value name="VALUE"><block type="variables_get"><title name="VAR">lista</title></block></value></block><block type="lists_getIndex"><value name="VALUE"><block type="variables_get"><title name="VAR">lista</title></block></value></block><block type="lists_setIndex"><value name="LIST"><block type="variables_get"><title name="VAR">lista</title></block></value></block><block type="lists_getSublist"><value name="LIST"><block type="variables_get"><title name="VAR">lista</title></block></value></block></category><category name="Variabler" custom="VARIABLE"></category><category name="Procedurer" custom="PROCEDURE"></category></xml>';
};