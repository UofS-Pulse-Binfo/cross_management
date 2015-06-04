<?php
/**
 * @file
 *
 */

/**
 * Form allowing breeders to set the crosses to be made
 * for a given crossing block.
 * @TODO: Extract data from Chado where necessary
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

  // Add Joyride Site Tour
  //--------------------------
  //$form['#attached']['library'][] = array('system', 'joyride');


  // Add Shepherd Site Tour
  //--------------------------
  $crossmanage_path = drupal_get_path('module', 'cross_manage');
  $shepherd_path = libraries_get_path('shepherd');
  $form['#attached']['js'][] = $shepherd_path . '/shepherd.min.js';
  $form['#attached']['js'][] = $crossmanage_path . '/theme/js/CrossManage_Parentsform_ShepherdTour.js';
  $form['#attached']['css'][] = $shepherd_path .'/css/shepherd-theme-arrows.css';
  $form['#attached']['css'][] = array(
    'data' => $crossmanage_path . '/theme/css/crossmanage_shepherd_tour.css',
    'weight' => 10
  );

  // Add site tour link.
  //--------------------------
  $form['start_tour'] = array(
    '#type' => 'markup',
    '#markup' => theme('cross_manage_shepherd_start_tour_link')
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

  // Begin parents table
  $parents_wrapper = 'parents-table';
  $form['parents'] = array(
    '#type' => ($cb_set) ? 'markup' : 'hidden',
    '#prefix' => '<div id="' . $parents_wrapper . '">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
    '#theme' => 'cross_manage_crossingblock_parents',
  );

  // @TODO: Set options to cross to based on rows that have been populated
  // (NOTE: Do we also want to pull options from the database?)
  //$parent_options = array(0 => '', 1 => 'P1', 2 => 'P2', 3 => 'P3');

  $num_rows = (isset($form_state['values'])) ? sizeof($form_state['values']['parents']) - 1 : 1;

  // Set the cross-to options - first value is blank so the drop-down is initially blank
  $parent_options = array(0 => '');
  for ($m=1; $m <= $num_rows; $m++) {
    if (isset($form_state['values']['parents'][$m]['line']) AND $form_state['values']['parents'][$m]['line'] != '') {
      $parent_options[] = $form_state['values']['parents'][$m]['line'];
    }
    else if (isset($form_state['values']['parents'][$m]['pnum'])) {
      $parent_options[] = 'P' . $form_state['values']['parents'][$m]['pnum'];
    }
  }

  // Only add an extra row if the "Add Parent" button was clicked
  // We also add the new option to the parent options in 'Cross To'
  // ASSUMPTION: P numbers are determined by the "nth" row value (ex: row 3 = P3)
  if (isset($form_state['clicked_button']) AND $form_state['clicked_button']['#value'] == 'Add Parent') {
    $num_rows++;
    $parent_options[] = 'P' . $num_rows;
  }

  // Use For loop to ensure we always get one more row than options.
  // NOTE: We didn't use +1 on existing options b/c there is an add-row element
  // for the button which effectively adds our +1.
  // ASSUMPTION: You will not add any elements to $form['parents'] other than
  // the rows and the add parent button. If you do,
  // (ie: $form['parents']['another button']) you will get two empty rows.
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
      '#size' => 2,
      '#required' => TRUE,
    );

    $form['parents'][$pnum]['line_nid'] = array(
      '#type' => 'hidden',
      '#value' => (isset($opt['line_nid'])) ? $opt['line_nid'] : NULL,
    );

    // @TODO Get the pedigree of the line if its available
    $more_info = "Unknown Line";
    if (!empty($opt['line_nid'])) {
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
      '#size' => 35,
      '#required' => TRUE,
    );

    $form['parents'][$pnum]['source'] = array(
      '#type' => 'textfield',
      //'#title' => 'Source',
      '#default_value' => $source,
      '#size' => 35,
      '#required' => TRUE,
    );

    $form['parents'][$pnum]['notes'] = array(
      '#type' => 'markup',
    );

    $form['parents'][$pnum]['notes']['breeder_notes'] = array(
      '#type' => 'textarea',
      '#attributes' => array('placeholder' => 'Breeder Notes'),
      '#rows' => 1,
    );

    $form['parents'][$pnum]['notes']['instructions'] = array(
      '#type' => 'textarea',
      '#attributes' => array('placeholder' => 'Instructions'),
      '#rows' => 1,
    );

    $cross_to_wrapper = 'cross-to-' . $pnum;
    $form['parents'][$pnum]['cross_to'] = array(
      '#type' => 'item',
      '#prefix' => '<div id="' . $cross_to_wrapper . '">',
      '#suffix' => '</div>',
      //'#title' => 'Cross To',
    );

    // Add a number of cross to dropdowns to allow breeders to specify what
    // parents they want to cross with the current line
    // NOTE: Ajax is used to ensure that there is always one more select box
    // than there are parents selected so that breeders can add more parents
    // to cross with.

    // Determine how many select boxes we need.
    $num_crosses = (!empty($form_state['values']['parents'][$pnum]['cross_to'])) ? sizeof($form_state['values']['parents'][$pnum]['cross_to']) : 0;

    for ($j = 0; $j <= $num_crosses; $j++) {
      // Determine our default value for the next select box.
      if (isset($form_state['values']['parents'][$pnum]['cross_to'][$j])) {
        // Default value is what has been previously entered
        $cross_to_default = $form_state['values']['parents'][$pnum]['cross_to'][$j];
        // If the final box is already empty, don't iterate through the loop a final time
        // Otherwise, another empty box would be added.
        if (empty($form_state['values']['parents'][$pnum]['cross_to'][$j]) AND ($j == ($num_crosses - 1))) {
          $num_crosses--;
        }
      }
      else { // This is a brand new select box, so make the default value empty.
        $cross_to_default = 0;
      }

      // Create the select box
      $form['parents'][$pnum]['cross_to'][] = array(
        '#type' => 'select',
        '#options' => $parent_options,
        '#default_value' => $parent_options[$cross_to_default],
        '#ajax' => array(
          'callback' => 'ajax_parents_cross_to_callback',
          'wrapper' => $cross_to_wrapper,
        ),
      );
    }
  }

  // Some ajax has been commented out that was meant to add a new row to the
  // parents table without losing any information that has already been entered.
  // This is currently being done with a form rebuild instead.
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
  * Form validation for breeders to set crosses.
  * Ensures a cross cannot occur with itself, nor can 2+ identical crosses occur
  * BUG: Since this code compares indices rather than values, it does not work properly
  *      when a breeder changes a P num manually. ALSO SEE: cross_manage_crossingblock_set_cross_f1s_form_validate
  * @TODO: Compare values rather than indices
  */
function cross_manage_crossingblock_set_cross_parents_form_validate($form, &$form_state) {

  // Validate each row in the same manner.
  foreach ($form_state['values']['parents'] as $pnum => $row) {

    // Ensure that the line entered is valid.
    // Note that this shouldn't be empty because it's marked required
    // but we are checking anyway since a database query is involved
    // and we all know there are sometimes #DrupalWTF moments ;).
    if (!empty($form_state['values']['parents'][$pnum]['line'])) {

      // @TODO: Allow uniquename, name, synonym, dbxref.
      $results = chado_select_record('stock', array('stock_id', 'uniquename'), array('uniquename' => $form_state['values']['parents'][$pnum]['line']));
      if (empty($results)) {
        form_set_error("parents][$pnum][line", "No Germplasm exists with that unique name.");
      }
      $form_state['values']['parents'][$pnum]['stock_id'] = $results[0]->stock_id;
    }

    // Validate the "Cross To" elements.
    if (isset($form_state['values']['parents'][$pnum]['cross_to'])) {
      // Check each selection to compare to the pnum, then to other selections
      foreach ($form_state['values']['parents'][$pnum]['cross_to'] as $k1 => $cross_to_num) {
        // If a selected cross-to matches the pnum, throw an error
        if ($pnum == $cross_to_num) {
          form_set_error('parents][' . $pnum . '][cross_to][' . $k1, 'Invalid cross: P' . $pnum . ' to P' . $pnum);
        }
        else {
          // If a selected cross-to is not empty, and is the same as a previously selected cross-to for that
          // pnum, throw an error.
          foreach ($form_state['values']['parents'][$pnum]['cross_to'] as $k2 => $cross_option) {
            if (!empty($form_state['values']['parents'][$pnum]['cross_to'][$k2]) AND ($cross_to_num == $cross_option) AND ($k1 != $k2)) {
              form_set_error('parents][' . $pnum . '][cross_to][' . $k2, 'Duplicate cross: P' . $pnum . ' to P' . $cross_to_num);
              //break 2; //breaking prevents two error messages from being seen but also highlights a single box from the duplicates
            }
          }
        }
      }
    }


  }
}

/**
 * Form submission for breeders to either submit the crossing block in its current state
 * or add another row for a new parent.
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
      dpm($form_state, 'Submit: Form State');

      // Add as a parent
      $record = array(
        'stock_id' => $form_state['values']['parents'][1]['stock_id'],
        'cb_year' => $form_state['build_info']['args'][1],
        'cb_season' => $form_state['build_info']['args'][2],
        'seed_source' => $form_state['values']['parents'][1]['source']
      );
      drupal_write_record('cross_manage_parents', $record);
    break;
  }
}

/**
function ajax_parents_add_row_callback($form, $form_state) {
  return $form['parents'];
}
*/

/**
 * Callback for ajax-enabled select boxes
 * Retrieves the pnum from the row that was "triggered" and returns the cross-to section
 * of the form for that row.
 */
function ajax_parents_cross_to_callback($form, $form_state) {
  $pnum = $form_state['triggering_element']['#array_parents'][1];
  return $form['parents'][$pnum]['cross_to'];
}

/**
 * Implements hook_form().
 * Form: Plan Crosses: F1's
 *
 * Form allowing breeders to indicate which germplasm should be crossed to the
 * F1s resulting from the previous crossing block. The ability to provide
 * additional instructions, such as seed designated for grow-outs and crosses,
 * is also available.
 */
function cross_manage_crossingblock_set_cross_f1s_form($form, &$form_state) {

  // Get the year/season for the PREVIOUS corssing block since that's
  // where the record for the F1's would be.
  // NOTE: To determine previous crossing block, we need to choose the previous
  // season and if the current season is spring then choose winter of the
  // previous year.
  $seasons = array('Spring', 'Summer', 'Fall', 'Winter');
  $curr_season_key = array_search($form_state['build_info']['args'][2], $seasons);
  if ($curr_season_key > 0) {
    $prev_cb_season = $seasons[$curr_season_key - 1];
    $prev_cb_year = $form_state['build_info']['args'][1];
  } else {
    $prev_cb_season = 'Winter';
    $prev_cb_year = $form_state['build_info']['args'][1] - 1;
  }

  // The following query pulls all F1s from the PREVIOUS crossing block.
  // NOTE: Where this becomes difficult is that the breeders don't put
  // in a crossing block every querter so if the query below returns 0 then
  // you need to grab for those for 2 crossing blocks ago.
  $f1s = chado_query("
    SELECT
      s.*,
      org.genus,
      org.species,
      type.name as type_name,
      mats.name||'/'||pats.name as pedigree,
      num_seeds.value as num_seeds
    FROM {stock} s
    LEFT JOIN {stockprop} cby ON s.stock_id=cby.stock_id AND cby.type_id=:cb_year_id
    LEFT JOIN {stockprop} cbs ON s.stock_id=cbs.stock_id AND cbs.type_id=:cb_season_id
    LEFT JOIN {organism} org ON org.organism_id=s.organism_id
    LEFT JOIN {cvterm} type ON type.cvterm_id=s.type_id
    LEFT JOIN {stock_relationship} mat ON mat.object_id=s.stock_id AND mat.type_id=:mat_type_id
    LEFT JOIN {stock} mats ON mats.stock_id=mat.subject_id
    LEFT JOIN {stock_relationship} pat ON pat.object_id=s.stock_id AND pat.type_id=:pat_type_id
    LEFT JOIN {stock} pats ON pats.stock_id=pat.subject_id
    LEFT JOIN {stockprop} num_seeds ON num_seeds.stock_id=s.stock_id AND num_seeds.type_id=:seeds_type_id
    WHERE cby.value=:cb_year AND cbs.value=:cb_season AND org.genus=:genus
    ORDER BY s.name ASC",
    array(
      'mat_type_id' => '3632',
      'pat_type_id' => '3633',
      'seeds_type_id' => '3973',
      'cb_year_id' => '3663',
      'cb_season_id' => '3664',
      'genus' => $form_state['build_info']['args'][0],
      'cb_year' => $prev_cb_year,
      'cb_season' => $prev_cb_season,
    )
  )->fetchAll();

  // Set Page Title
  $species = str_replace('-',' ',$form_state['build_info']['args'][0]);
  $cb_year = $form_state['build_info']['args'][1];
  $cb_season = $form_state['build_info']['args'][2];
  drupal_set_title(format_string('Plan Crosses with F1s from: @species, @season @year Crossing Block', array(
      '@species' => $species,
      '@year' => $cb_year,
      '@season' => $cb_season,
  )));

  // Add Joyride Site Tour
  //--------------------------
  //$form['#attached']['libraries_load'][] = array('joyride');

  // Add Shepherd Site Tour
  //--------------------------
  $crossmanage_path = drupal_get_path('module', 'cross_manage');
  $shepherd_path = libraries_get_path('shepherd');
  $form['#attached']['js'][] = $shepherd_path . '/shepherd.min.js';
  $form['#attached']['js'][] = $crossmanage_path . '/theme/js/CrossManage_F1form_ShepherdTour.js';
  $form['#attached']['css'][] = $shepherd_path .'/css/shepherd-theme-arrows.css';
  $form['#attached']['css'][] = array(
    'data' => $crossmanage_path . '/theme/css/crossmanage_shepherd_tour.css',
    'weight' => 10
  );

  // Add site tour link.
  //--------------------------
  $form['start_tour'] = array(
    '#type' => 'markup',
    '#markup' => theme('cross_manage_shepherd_start_tour_link')
  );

  // css which allows form item titles to float left of the text box to save space,
  // as well as for text boxes within the same item to align
  // ie: seed allocation fields
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
      td.type, th.type {
        width: 100px;
      }
      td.seed_num, th.seed_num {
        width: 90px;
        text-align: center;
      }
      td.seed_num .form-item.form-type-textfield {
        text-align: center;
      }
      td.seed_allocate, th.seed_allocate {
        width: 100px;
      }
    '
  );

  // PARENT OPTIONS
  // Currently includes all F1s
  // @TODO: Also should be able to select crosses resulting from the parents form?
  //        To pnum or not to pnum here?
  $parent_options = array('');
  foreach ($f1s as $k => $details) {
    $parent_options[] = $details->name;
  }
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
  foreach ($f1s as $cross_num => $details) {

    $form['f1'][$cross_num]['cross_num'] = array(
      '#type' => 'item',
      //'#title' => 'Cross Number',
      '#markup' => $details->name,
    );

    $form['f1'][$cross_num]['type'] = array(
      '#type' => 'item',
      //'#title' => 'Type',
      '#markup' => $details->type_name,
    );

    $form['f1'][$cross_num]['pedigree'] = array(
      '#type' => 'item',
      //'#title' => 'Pedigree',
      '#markup' => $details->pedigree,
    );

    // Check to see if we know the number of seeds produced from the previous crossing block
    // for this F1. If not, display a textbox to allow breeders to enter seed amount if they
    // know it (ie. OPTIONAL).
    if(isset($details->num_seeds)) {
      $form['f1'][$cross_num]['seed_num'] = array(
        '#type' => 'item',
        //'#title' => 'Total Seed Produced',
        '#markup' => $details->num_seeds,
      );
    }
    else {
      $form['f1'][$cross_num]['seed_num'] = array(
        '#type' => 'textfield',
        '#attributes' => array('placeholder' => 'Unknown'),
        '#size' => '7',
      );
    }

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

    /** MARKERS
     * Add the ability to select markers to screen for each F1. Clicking on '+' would
     * bring up a popup window to allow breeders to browse/select markers.
     * @TODO: Implement marker selection
    /*
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
      '#attributes' => array('placeholder' => 'Breeder Notes'),
      '#rows' => 1
    );

    $form['f1'][$cross_num]['notes']['instructions'] = array(
      '#type' => 'textarea',
      '#attributes' => array('placeholder' => 'Instructions'),
      '#rows' => 1
    );

    // Set up a wrapper to allow ajax to replace this item
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
    // than there are parents selected so that breeders can add more parents.

    // Determine how many select boxes we need.
    $num_crosses = (!empty($form_state['values']['f1'][$cross_num]['cross_to'])) ? sizeof($form_state['values']['f1'][$cross_num]['cross_to']) : 0;
    /*
    // Ensure that the triggering element is limited to a select box from within the row,
    // and if so, increment the number of crosses
    if (isset($form_state['triggering_element'])
      AND isset($form_state['triggering_element']['#id'])
      AND preg_match('/^edit-f1-' . $i . '/', $form_state['triggering_element']['#id']))
    {
      $num_crosses++;
    }
    */
    for ($j = 0; $j <= $num_crosses; $j++) {
      // Determine our default value for the next select box.
      if (isset($form_state['values']['f1'][$cross_num]['cross_to'][$j])) {
        // Default value is what has been previously entered
        $cross_to_default = $form_state['values']['f1'][$cross_num]['cross_to'][$j];
        // If the final box is already empty, don't iterate through the loop a final time
        // Otherwise, another empty box would be added.
        if (empty($form_state['values']['f1'][$cross_num]['cross_to'][$j]) AND ($j == ($num_crosses - 1))) {
          $num_crosses--;
        }
      }
      else { // This is a brand new select box, so make the default value empty.
        $cross_to_default = '';
      }

      // Then create a select box for each previously specified cross + one blank
      $form['f1'][$cross_num]['cross_to'][] = array(
        '#type' => 'select',
        '#options' => $parent_options,
        '#default_value' => $parent_options[$cross_to_default],
        '#ajax' => array(
          'callback' => 'ajax_f1_cross_to_callback',
          'wrapper' => $cross_to_wrapper,
        ),
      );
    }
  }

  // Should type here be button?
  $form['save-f1'] = array(
    '#type' => 'submit',
    '#value' => 'Save Progress',
  );

  $form['submit-f1'] = array(
    '#type' => 'submit',
    '#value' => 'Send to Field Lab',
  );

/**
  $form['joyride'] = array(
    '#type' => 'markup',
    '#markup' => theme('cross_manage_crossingblock_f1_joyride')
  );
*/

  return $form;
}

/**
 * Form validation for breeders to set crosses with F1s.
 * Ensures a cross cannot occur with itself, nor can 2+ identical crosses occur
 * BUG: Does not work as expected despite being the same code as for the Parents form
 * @TODO: Compare values rather than indices
 */
function cross_manage_crossingblock_set_cross_f1s_form_validate($form, &$form_state) {
  foreach ($form_state['values']['f1'] as $cross_num => $elements) {
    if (isset($form_state['values']['f1'][$cross_num]['cross_to'])) {
      // Check each selection to compare to the cross_num, then to other selections
      foreach ($form_state['values']['f1'][$cross_num]['cross_to'] as $k1 => $cross_to_num) {
        // If a selected cross-to matches the cross number, throw an error
        if ($cross_num == $cross_to_num) {
          form_set_error('f1][' . $cross_num . '][cross_to][' . $k1, 'Invalid cross: ' . $cross_num . ' to ' . $cross_num);
        }
        else {
          // If a selected cross-to is not empty, and is the same as a previously selected cross-to for that
          // cross number, throw an error.
          foreach ($form_state['values']['f1'][$cross_num]['cross_to'] as $k2 => $cross_option) {
            if (!empty($form_state['values']['f1'][$cross_num]['cross_to'][$k2]) AND ($cross_to_num == $cross_option) AND ($k1 != $k2)) {
              form_set_error('f1][' . $cross_num . '][cross_to][' . $k2, 'Duplicate cross: ' . $cross_num . ' to ' . $cross_to_num);
              //break 2; //breaking prevents two error messages from being seen but also highlights a single box from the duplicates
            }
          }
        }
      }
    }
  }
}

/**
 * Form submission for breeders to submit F1 crosses to field lab.
 *
 * Implements hook_form_submit().
 */
function cross_manage_crossingblock_set_cross_f1s_form_submit($form, $form_state) {
  drupal_set_message('Successfully submitted F1s form to Field Lab');
}

/**
 * Callback for ajax-enabled select boxes
 * Retrieves the cross_num from the row that was "triggered" and returns the cross-to section
 * of the form for that row.
 */
function ajax_f1_cross_to_callback($form, $form_state) {
  $cross_num = $form_state['triggering_element']['#array_parents'][1];
  return $form['f1'][$cross_num]['cross_to'];
}
