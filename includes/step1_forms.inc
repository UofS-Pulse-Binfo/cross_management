<?php
/**
 * @file
 *
 */

/**
 * MOCK-UP: Form allowing breeders to set the crosses to be made
 * for a given crossing block.
 *
 * Implements hook_form().
 */
function cross_manage_crossingblock_set_cross_parents_form($form, &$form_state) {

  // DEFAULTS:
  //--------------------------
  // If the crossing block is already set
  if (sizeof($form_state['build_info']['args']) >=3 ) {
    // Crossing block
    $cb_set = TRUE;
    $species = str_replace('-',' ',$form_state['build_info']['args'][0]);
    $cb_year = $form_state['build_info']['args'][1];
    $cb_season = $form_state['build_info']['args'][2];

    // Set Page Title
    drupal_set_title(format_string('Plan Crosses: @species, @season @year Crossing Block', array(
        '@species' => $species,
        '@year' => $cb_year,
        '@season' => $cb_season,
    )));

    $highlight_options = array(
      'market_class' => 'Market Class'
    );

    // What to highlight the rows based on
    $highlight_detail = 'market_class';

    $marker_data_avail = FALSE;
    if (isset($form_state['build_info']['args']['markers_screened'])) {
      $marker_data_avail = TRUE;
    }
  }
  else {
    $cb_set = FALSE;
    $species = NULL;
    $cb_year = date("Y");
    $cb_season = NULL;
  }

  // Add Themeing:
  //--------------------------
  $path = drupal_get_path('module', 'cross_manage');
  $form['#attached'] = array(
    'css' => array(
      'type' => 'file',
      'data' => $path . '/theme/css/cross_manage.css',
    ),
  );

  // FORM PROPER:
  //--------------------------
/**
  $form['highlight'] = array(
    '#type' => 'fieldset',
    '#title' => 'Row Highlight'
  );

  $form['highlight']['highlight_select'] = array(
    '#type' => 'select',
    '#title' => 'Highlight Row',
    '#description' => 'Select the detail you wish to highlight the rows based on.',
    '#options' => $highlight_options,
    '#default_value' => $highlight_detail,
    '#prefix' => '<span class="cross-parents highlight highlight-select">',
    '#suffix' => '</span>'
  );

  $form['highlight']['highlight_key'] = array(
    '#type' => 'markup',
    '#markup' => "<table>
                    <caption><strong>Highlight Colour Key</strong></caption>
                    <tr><td class='highlight-key Small-Red'></td><td>Small Red</td></tr>
                    <tr><td class='highlight-key French-Green'></td><td>French Green</td></tr>
                  </table>",
    '#prefix' => '<span class="cross-parents highlight highlight-key">',
    '#suffix' => '</span>'
  );
*/
  $parents_wrapper = 'parents-table';
  $form['parents'] = array(
    '#type' => ($cb_set) ? 'markup' : 'hidden',
    '#prefix' => '<div id="' . $parents_wrapper . '">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
    '#theme' => 'cross_manage_crossingblock_parents',
  );

  $parent_options = array(0 => '', 1 => 'P1', 2 => 'P2', 3 => 'P3');

  // When ajax updates the form to add a parent, keep all previously entered values. Otherwise,
  // populate the form with mock_options

  // Use For loop to ensure we always get one more row then there are options.
  // NOTE: We didn't use +1 on existing options b/c there is an add-row element
  // for the button which effectively adds our +1.
  // ASSUMPTION: You will not add any elements to $form['parents'] other then
  // the rows and the add parent button. If you do
  // (ie: $form['parents']['another button']) you will get two empty rows.
  $num_rows = (isset($form_state['values'])) ? sizeof($form_state['values']['parents']) - 1 : 1;
  if (isset($form_state['clicked_button']) AND $form_state['clicked_button']['#value'] == 'Add Parent') {
    $num_rows++;
  }
  for ($i=1; $i <= $num_rows; $i++) {

    // Either grab the existing options from the form state
    if (isset($form_state['values']['parents'][$i])) {
      $opt = $form_state['values']['parents'][$i];
      $pnum = $opt['pnum'];
      $line = $opt['line'];
      $source = $opt['source'];
      $cross_to = $opt['cross_to'];
      $line_not_valid = (isset($opt['line_not_valid'])) ? $opt['line_not_valid']: FALSE;
      $line_validitity = ($line_not_valid) ? 'line-invalid' : 'line-valid';
    }
    // OR if this is the last row then there are no existing values
    // so set blank defaults.
    else {
      $pnum = $i;
      $line = '';
      $source = '';
      $cross_to = array();
      $details = '';
      $line_not_valid = FALSE;
    }

    $highlight_class = 'no-highlight';
    if (isset($opt['details'][$highlight_detail])) {
      $highlight_class = str_replace(' ','-', $opt['details'][$highlight_detail]['value']);
    }

    $form['parents'][$pnum] = array(
      '#type' => 'markup',
      //'#prefix' => '<span class="cross-parents row '.$highlight_class.'">',
      //'#suffix' => '</span>'
    );
    if ($i == ($num_rows - 1)) {
      $form['parents'][$pnum]['#prefix'] = '<span class="cross-parents row last">';
    }

    $form['parents'][$pnum]['highlight_class'] = array(
      '#type' => 'item',
      '#value' => $highlight_class
    );

    $form['parents'][$pnum]['pnum'] = array(
      '#type' => 'textfield',
      //'#title' => 'Parent Number',
      '#default_value' => $pnum,
      '#prefix' => '<span class="pre-label">P</span>',
      '#size' => 2
    );

    $more_info = "Unknown Line";
    if (!empty($details)) {
      $link = l(
        'Full Listing',
        'node/'.$opt['line_nid'],
        array('attributes' => array('target'=>'_blank'))
      );
      $more_info = '<span class="line-details">';
      $more_info .= $details['PEDIGREE']['value'].' ('.$link.')';
      $more_info .= '</span>';
    }
    $form['parents'][$pnum]['line'] = array(
      '#type' => 'textfield',
      //'#title' => 'Line',
      '#default_value' => $line,
      '#description' => $more_info,
      '#size' => 35
    );

    $form['parents'][$pnum]['source'] = array(
      '#type' => 'textfield',
      //'#title' => 'Source',
      '#default_value' => $source,
      '#size' => 35
    );

    $form['parents'][$pnum]['notes'] = array(
      '#type' => 'markup'
    );

    $form['parents'][$pnum]['notes']['breeder_notes'] = array(
      '#type' => 'textarea',
      '#default_value' => 'Breeder Notes',
      '#rows' => 1
    );

    $form['parents'][$pnum]['notes']['instructions'] = array(
      '#type' => 'textarea',
      '#default_value' => 'Instructions',
      '#rows' => 1
    );

    $cross_to_wrapper = 'cross-to-' . $pnum;
    $form['parents'][$pnum]['cross_to'] = array(
      '#type' => 'item',
      '#prefix' => '<div id="' . $cross_to_wrapper . '">',
      '#suffix' => '</div>',
      //'#title' => 'Cross To',
    );

    // Add a number of cross to dropdowns to allow breeders to specify what
    // parents they want to cross with the current F1
    // NOTE: Ajax is used to ensure that there is always one more select box
    // than there are parents selected to ensure that breeders can add more parents.
    // @TODO: Implement ajax.
    $existing_cross_to = (!empty($form_state['values']['parents'][$pnum]['cross_to'])) ? $form_state['values']['parents'][$pnum]['cross_to'] : array();

    $num_crosses = (!empty($form_state['values']['parents'][$pnum]['cross_to'])) ? sizeof($form_state['values']['parents'][$pnum]['cross_to']) : 1;
    if (isset($form_state['triggering_element'])
      AND isset($form_state['triggering_element']['#id'])
      AND preg_match('/^edit-parents-' . $i . '/', $form_state['triggering_element']['#id']))
    {
      $num_crosses++;
    }
    for ($j = 1; $j <= $num_crosses; $j++) {
      if ($j == $num_crosses) {
        $cross_to_default = NULL;
      }
      else {
        $cross_to_default = $form_state['values']['parents'][$pnum]['cross_to'][$j];
      }
      $form['parents'][$pnum]['cross_to'][] = array(
        '#type' => 'select',
        '#options' => $parent_options,
        '#default_value' => $cross_to_default,
        '#ajax' => array(
          'callback' => 'ajax_parents_cross_to_callback',
          'wrapper' => $cross_to_wrapper,
        ),
      );
    }

  }

  // Implement ajax to add a new row to the parents table without losing any
  // information that has already been entered.
  $form['parents']['add-row'] = array(
    '#type' => 'submit',
    '#value' => 'Add Parent',
    '#prefix' => '<span class="cross-parents add-more">',
    '#suffix' => '</span>',
    /**
    '#ajax' => array(
      'callback' => 'ajax_parents_add_row_callback',
      'wrapper' => $parents_wrapper,
    ),*/
  );

  $form['submit-cb'] = array(
    '#type' => 'submit',
    '#value' => 'Submit Crossing Plans',
  );

  return $form;
}

/**
  * MOCK-UP Form validation for breeders to set crosses
  *
  *
  *
  */
function cross_manage_crossingblock_set_cross_parents_form_validate($form, &$form_state) {

//dpm($form, 'form in validate');
//dpm($form_state, 'form state in validate');

  if ($form_state['triggering_element']['#type'] == 'select') {
    foreach ($form_state['values']['parents'] as $pnum => $elements) {
      $form_state['values']['parents'][$pnum]['cross_to'] = array_filter($elements['cross_to']);
    }
  }
}

/**
 * MOCK-UP: Form allowing breeders to set the crosses to be made
 * for a given crossing block.
 *
 * Implements hook_form_submit().
 */
function cross_manage_crossingblock_set_cross_parents_form_submit(&$form, &$form_state) {

  switch ($form_state['clicked_button']['#value']) {
    case 'Submit Crossing Plans':
      $form_state['rebuild'] = FALSE;
      drupal_set_message('Successfully submitted crossing plans');
    break;
    case 'Add Parent':
      $form_state['rebuild'] = TRUE;
    break;
  }
}


function ajax_parents_add_row_callback($form, $form_state) {
  return $form['parents'];
}


function ajax_parents_cross_to_callback($form, $form_state) {
  $pnum = $form_state['triggering_element']['#array_parents'][1];
  return $form['parents'][$pnum]['cross_to'];
}

/**
 * Implements hook_form().
 * Form: Plan Crosses: F1's
 *
 * Form allowing breeders to indicate which germplasm should be crossed to the
 * F1's resulting from the previous crossing block. The ability to provide
 * additional instructions including which F1s to grow-out is also available.
 */
function cross_manage_crossingblock_set_cross_f1s_form($form, &$form_state) {

//dpm($form_state, 'form_state');

  // My Hardcoded F1 data for the example.
  // @TODO: Select this data from chado for the crossing block previous to the current one.
  $mock = array(
    '2604S' => array('2604S','SX','NY5-161 White/Jetwhite4-11-21','20',array('2605S','P6','P7')),
    '2605S' => array('2605S','SX','NY5-161 White/WC-BC1-1','21',array('2604S')),
    '2606S' => array('2606S','SX','NY5-161 Brown/Jetblack4-15-3','22',array('2608S','P4')),
    '2607aS' => array('2607aS','SX','NY5-161 Brown/WC-BC1-1','23',array('P5')),
    '2608S' => array('2608S','SX','G9345/Jetblack4-11-11','24',array()),
    '2610bS' => array('2610bS','SX','1533a-15/Higuera-E','25',array()),
    '2613bS' => array('2613bS','SX','GDR6aa/737-22','26',array('P12','P13','P15','P17')),
    '2615S' => array('2615S','SX','316-13/Jetwhite4-11-21','27',array('P31')),
    '2616S' => array('2616S','SX','1441-8/Jetwhite4-11-21','28',array('P4')),
    '2617S' => array('2617S','SX','Expresso-5/Jetwhite4-11-21','29',array('P5')),
    '2618S' => array('2618S','SX','1780TF3-1/Jetwhite4-11-21','30',array('P6','P7')),
    '2619S' => array('2619S','SX','GDR32a-2/Jetwhite4-11-21','25',array('P12','P13')),
    '2620bS' => array('2620bS','SX','316-13/Jetblack4-15-3','26',array('P31')),
    '2621S' => array('2621S','SX','1441-8/Jetblack4-15-3','27',array('2622S')),
    '2622S' => array('2622S','SX','Expresso-5/Jetblack4-15-3','28',array('P4','P5'))
  );

  $form['#attached']['css'][] = array(
    'type' => 'inline',
    'data' => '
      .form-item.form-type-textfield {
        text-align: right;
      }
      .form-item label {
        display:inline-block;
        width:55px;
        text-align:right;
      }
      .form-text {
        display:inline;
        width:auto;
      }
    '
  );

  // My Hardcoded Parents for the example.
  // These parents were added for this crossing block using the parents form
  // defined in cross_manage_crossingblock_set_cross_parents_form().
  // @TODO: Select these from the chado stock table.
  $parent_options = array('', '2604S', '2608S', '2622S', 'P4','P5','P6','P7','P12','P13','P15','P17','P31');
  $parent_options = array_combine($parent_options, $parent_options);

  $form['f1'] = array(
    '#type' => 'markup',
    '#title' => "F1's",
    // We denote this fieldset as a tree to ensure that the data is nested
    // when it makes it to the submit function.
    '#tree' => TRUE,
    '#theme' => 'cross_manage_crossingblock_f1'
  );

  // For each F1 we want to create a single row in the data entry table
  // allowing breeders to specify instructions per F1.
  // NOTE: titles are commented out to make things easier in the theme function
  // where these plain form fields will be rendered in a table layout.
  foreach ($mock as $cross_num => $details) {
    $form['f1'][$cross_num]['cross_num'] = array(
      '#type' => 'item',
      //'#title' => 'Cross Number',
      '#markup' => $details[0],
    );

    $form['f1'][$cross_num]['type'] = array(
      '#type' => 'item',
      //'#title' => 'Type',
      '#markup' => $details[1]
    );

    $form['f1'][$cross_num]['pedigree'] = array(
      '#type' => 'item',
      //'#title' => 'Pedigree',
      '#markup' => $details[2],
    );

   // The 1st way to represent number of seeds for 1) crossing block, 2) reserve, 3) grow-out.
    $form['f1'][$cross_num]['seed_num'] = array(
      '#type' => 'item',
      //'#title' => 'Total Seed Count',
      '#markup' => $details[3],
    );

    $form['f1'][$cross_num]['seed_allocate']['cross'] = array(
      '#type' => 'textfield',
      '#title' => t("Cross:"),
      '#size' => 3,
    );

    $form['f1'][$cross_num]['seed_allocate']['reserve'] = array(
      '#type' => 'textfield',
      '#title' => t("Reserve:"),
      '#size' => 3,
    );

    $form['f1'][$cross_num]['seed_allocate']['grow-out'] = array(
      '#type' => 'textfield',
      '#title' => t('Grow-out:'),
      '#size' => 3,
    );

/**
   // The 2nd way to represent number of seeds for 1) crossing block, 2) reserve, 3) grow-out.
    $form['f1'][$cross_num]['seed_num'] = array(
      '#type' => 'item',
      //'#title' => 'Total Seed Count',
      '#markup' => $details[3],
    );

    $form['f1'][$cross_num]['seed2cross'] = array(
      '#type' => 'textfield',
      //'#title' => 'Seed to Cross',
      '#size' => 3,
      '#default_value' => $details[3],
    );

    $form['f1'][$cross_num]['seed2reserve'] = array(
      '#type' => 'textfield',
      //'#title' => 'Seed to Reserve',
      '#size' => 3,
      '#default_value' => '0',
    );

    $form['f1'][$cross_num]['seed2growout'] = array(
      '#type' => 'textfield',
      //'#title' => 'Seed to Grow-out',
      '#size' => 3,
      '#default_value' => '0',
    );
*/

    // Add the ability to select markers to screen for each F1. Clicking on '+' would
    // bring up a popup window to allow breeders to browse/select markers.
    // @TODO: Implement marker selection
    /**
    $form['f1'][$cross_num]['marker_data']['markers2screen'] = array(
      '#type' => 'markup',
      //'#title' => 'Markers to Screen Already Selected',
      '#markup' => 'marker1, marker3, marker4'
        . '<img src="http://icons.iconarchive.com/icons/visualpharm/icons8-metro-style/256/Very-Basic-Plus-icon.png" height="16" weight="16" style="border:none;">'
        . ' '
        . '<img src="http://icons.iconarchive.com/icons/visualpharm/icons8-metro-style/256/Very-Basic-Minus-icon.png" height="16" weight="16" style="border:none;">'
    );
    */

    $form['f1'][$cross_num]['notes']['breeder_notes'] = array(
      '#type' => 'textarea',
      '#default_value' => 'Breeder Notes',
      '#rows' => 1
    );

    $form['f1'][$cross_num]['notes']['instructions'] = array(
      '#type' => 'textarea',
      '#default_value' => 'Instructions',
      '#rows' => 1
    );

    // Not an ideal way for breeders to specify reserve seed for grow-outs
//     $form['f1'][$cross_num]['notes']['grow_out'] = array(
//       '#type' => 'checkbox',
//       '#title' => 'Grow-out?',
//       '#default_value' => TRUE,
//     );

    $cross_to_wrapper = 'cross-to-' . $cross_num;
    $form['f1'][$cross_num]['cross_to'] = array(
      '#type' => 'item',
      '#prefix' => '<div id="' . $cross_to_wrapper . '">',
      '#suffix' => '</div>',
      //'#title' => 'Cross To',
    );

    // Add a number of cross to dropdowns to allow breeders to specify what
    // parents they want to cross with the current F1
    // NOTE: Ajax is used to ensure that there is always one more select box
    // than there are parents selected to ensure that breeders can add more parents.
    // @TODO: Implement ajax.
    $existing_cross_to = (!empty($form_state['values']['f1'][$cross_num]['cross_to'])) ? $form_state['values']['f1'][$cross_num]['cross_to'] : array();

    foreach ($existing_cross_to as $cross_to_default) {
      //if (!empty($cross_to_default)) {
        // Single select box with default set to value from form_state.
        $form['f1'][$cross_num]['cross_to'][] = array(
          '#type' => 'select',
          '#options' => $parent_options,
          '#default_value' => $cross_to_default,
          '#ajax' => array(
            'callback' => 'ajax_f1_cross_to_callback',
            'wrapper' => $cross_to_wrapper,
          ),
        );
      //}
    }

    // Then create an additional select to allow them to add a new cross.
    $form['f1'][$cross_num]['cross_to'][] = array(
      '#type' => 'select',
      '#options' => $parent_options,
      '#ajax' => array(
        'callback' => 'ajax_f1_cross_to_callback',
        'wrapper' => $cross_to_wrapper,
      ),
    );

    $form['submit-f1'] = array(
      '#type' => 'submit',
      '#value' => 'Send to Field Lab',
    );

  }
  return $form;
}

/**
 * MOCK-UP Form allowing breeders to set the crosses to be made
 * for a given crossing block.
 *
 * Implements hook_form_submit().
 */
function cross_manage_crossingblock_set_cross_f1s_form_validate($form, &$form_state) {
  foreach ($form_state['values']['f1'] as $cross_num => $elements) {
    $form_state['values']['f1'][$cross_num]['cross_to'] = array_filter($elements['cross_to']);
  }
}

/**
 * MOCK-UP Form allowing breeders to set the crosses to be made
 * for a given crossing block.
 *
 * Implements hook_form_submit().
 */
function cross_manage_crossingblock_set_cross_f1s_form_submit($form, $form_state) {
  drupal_set_message('Successfully submitted F1s form to Field Lab');
}

function ajax_f1_cross_to_callback($form, $form_state) {
  $cross_num = $form_state['triggering_element']['#array_parents'][1];
  return $form['f1'][$cross_num]['cross_to'];
}
