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
    drupal_set_message('Unable to determine the current crossing block', 'error');
    return $form;
  }

	// Set Breadcrumb:
	//--------------------------
	$breadcrumb = array();
	$breadcrumb[] = l('Home', '<front>');
	$breadcrumb[] = l('Crossing Block Management', 'crossing-block');
	$breadcrumb[] = l($cb_season. ' ' . $cb_year, "crossing-block/$species/$cb_year/$cb_season/summary");
	$breadcrumb[] = l('Plan Crosses: Parents', base_path() . request_uri()); // Link to current URL
	drupal_set_breadcrumb($breadcrumb);

  // Add Themeing:
  //--------------------------
  $crossmanage_path = drupal_get_path('module', 'cross_manage');
  $form['#attached'] = array(
    'css' => array(
      'type' => 'file',
      'data' => $crossmanage_path . '/theme/css/cross_manage_forms.css',
    ),
  );
  $form['#attached']['js'][] = $crossmanage_path . '/theme/js/CrossManage_highlightErrorRow.js';


  // Add Shepherd Site Tour
  //--------------------------
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

  // We need to grab the parents for this crossingblock.
  // Always pull the parents from the database since they should be saved on
  // addition of a parent. This is consistent and reliable.
  $parents = db_query("
    SELECT p.*, s.name, s.uniquename, cs.nid,
      self.value as breeder_notes, instruct.value as instructions,
      (SELECT array_agg(paternal_parent_id) FROM cross_manage_planned_crosses WHERE maternal_parent_id=p.cb_parent_id) as cross_to
    FROM {cross_manage_parents} p
    LEFT JOIN chado.stock s ON s.stock_id=p.stock_id
    LEFT JOIN {chado_stock} cs ON cs.stock_id = p.stock_id
    LEFT JOIN {cross_manage_notes} self ON self.cb_parent_id=p.cb_parent_id AND self.type='breeder_selfnote'
    LEFT JOIN {cross_manage_notes} instruct ON instruct.cb_parent_id=p.cb_parent_id AND instruct.type='fieldlab_instruction'
    WHERE cb_year=:year AND cb_season=:season AND cb_species=:species AND parent_number != 'F1'
    ORDER BY parent_number",
    array(':year' => $cb_year, ':season' => $cb_season, ':species' => $species))->fetchAll();
  usort($parents, 'cross_manage_sort_parents_by_pnumber');
  $num_rows = sizeof($parents);

  // Set the cross-to options - first value is blank so the drop-down is initially blank
  $parent_options = cross_manage_get_parent_options($species, $cb_year, $cb_season);

  $max_pnum = 0;
  for ($i=0; $i <= $num_rows; $i++) {

    // Either grab the existing options from the form state
    if (isset($parents[$i])) {

      $pid = $parents[$i]->cb_parent_id;

      $has_form_state = (isset($form_state['values']['parents'][$pid])) ? TRUE : FALSE;

      $pnum = ($has_form_state) ? $form_state['values']['parents'][$pid]['pnum'] : $parents[$i]->parent_number;
      $line = ($has_form_state) ? $form_state['values']['parents'][$pid]['line'] : $parents[$i]->name;
      $source = ($has_form_state) ? $form_state['values']['parents'][$pid]['source'] : $parents[$i]->seed_source;
      $breeder_notes = ($has_form_state) ? $form_state['values']['parents'][$pid]['notes']['breeder_notes'] : $parents[$i]->breeder_notes;
      $instructions = ($has_form_state) ? $form_state['values']['parents'][$pid]['notes']['instructions'] : $parents[$i]->instructions;

      $cross_to = array();
      if (!empty($parents[$i]->cross_to)) {
        if (!is_array($parents[$i]->cross_to)) {
          $parents[$i]->cross_to = explode("," , trim($parents[$i]->cross_to, '{}'));
        }
        $cross_to = $parents[$i]->cross_to;
      }
      $cross_to = ($has_form_state AND $form_state['values']['parents'][$pid]['cross_to'][0] != 0) ? $form_state['values']['parents'][$pid]['cross_to'] : $cross_to;

      $details = 'Germplasm Page';
      if (isset($parents[$i]->nid)) {
        $details = l($details, 'node/'.$parents[$i]->nid, array('attributes' => array('target' => '_blank')));
      }

      // Keep track of the maximum parent number in order to autofill the last row.
      $max_pnum = ($pnum > $max_pnum) ? $pnum : $max_pnum;
    }
    // OR if this is the last row then there are no existing values
    // so set blank defaults.
    else {
      $pid = 'new';
      $pnum = $max_pnum + 1;
      $line = '';
      $source = '';
      $cross_to = array();
      $details = '';
      $line_not_valid = FALSE;
      $breeder_notes = '';
      $instructions = '';
    }

    $highlight_class = 'no-highlight';
    if (isset($opt['details'][$highlight_detail])) {
      $highlight_class = str_replace(' ','-', $opt['details'][$highlight_detail]['value']);
    }

    $form['parents'][$pid] = array(
      '#type' => 'markup',
    );

    $form['parents'][$pid]['highlight_class'] = array(
      '#type' => 'item',
      '#value' => $highlight_class
    );

    $form['parents'][$pid]['pnum'] = array(
      '#type' => 'textfield',
      //'#title' => 'Parent Number',
      '#default_value' => $pnum,
      '#prefix' => '<span class="pre-label">P</span>',
      '#size' => 2,
      '#required' => TRUE,
    );

    $form['parents'][$pid]['cb_parent_id'] = array(
      '#type' => 'hidden',
      '#value' => $pid
    );

    $form['parents'][$pid]['line'] = array(
      '#type' => 'textfield',
      //'#title' => 'Line',
      '#default_value' => $line,
      '#description' => $details,
      '#size' => 35,
      //'#required' => TRUE, * Only Required on add parent thus must check ourselves.
      '#autocomplete_path' => 'tripal_ajax/cross_manage/stock_name/'.$form_state['build_info']['args'][0]
    );

    $form['parents'][$pid]['source'] = array(
      '#type' => 'textfield',
      //'#title' => 'Source',
      '#default_value' => $source,
      '#size' => 35,
      //'#required' => TRUE, * Only Required on add parent thus must check ourselves.
    );

    $form['parents'][$pid]['notes'] = array(
      '#type' => 'markup',
    );

    // We want to expand the size of the textarea based on the number of lines
    // up to a max of 3 lines. First we use a simple regex to count the number
    // of user defined lines. However, a user could type a paragraph that spans
    // multiple lines (not detected by regex) so we also use a character count
    // if the number of lines is determined to be one or less.
    $num_lines = preg_match_all('/(\r|\n|\<br\/\>|\<b\/\>|\<br \/\>)/', $breeder_notes);
    $num_lines = ($num_lines <= 1) ? ceil(strlen($breeder_notes)/75) : $num_lines;
    $length = ($num_lines >= 3) ? 3 : $num_lines;
    $length = ($length == 0) ? 1 : $length;

    $form['parents'][$pid]['notes']['breeder_notes'] = array(
      '#type' => 'textarea',
      '#attributes' => array('placeholder' => 'Breeder Notes'),
      '#rows' => $length,
      '#default_value' => $breeder_notes,
    );

    // Used the same logic as described for the number of rows for the
    // Breeder's notes textarea above.
    $num_lines = preg_match_all('/(\r|\n|\<br\/\>|\<b\/\>|\<br \/\>)/', $instructions);
    $num_lines = ($num_lines <= 1) ? ceil(strlen($instructions)/75) : $num_lines;
    $length = ($num_lines >= 3) ? 3 : $num_lines;
    $length = ($length == 0) ? 1 : $length;

    $form['parents'][$pid]['notes']['instructions'] = array(
      '#type' => 'textarea',
      '#attributes' => array('placeholder' => 'Instructions'),
      '#rows' => $length,
      '#default_value' => $instructions,
    );

    $cross_to_wrapper = 'cross-to-' . $pid;
    $form['parents'][$pid]['cross_to'] = array(
      '#type' => 'item',
      '#prefix' => '<div id="' . $cross_to_wrapper . '">',
      '#suffix' => '</div>',
      //'#title' => 'Cross To',
    );

    // Add a number of cross to dropdowns to allow breeders to specify what
    // parents they want to cross with the current line
    // Ajax is used to ensure that there is always one more select box
    // than there are parents selected so that breeders can add more parents
    // to cross with.

    // Determine how many select boxes we need.
    // We don't need to add one because we are using the 0-index where
    // size starts at 1.
    $num_crosses = (!empty($cross_to)) ? sizeof($cross_to) : 0;

    for ($j = 0; $j <= $num_crosses; $j++) {
      // Determine our default value for the next select box.
      if (isset($cross_to[$j])) {
        // Default value is what has been previously entered
        $cross_to_default = $cross_to[$j];
        // If the final box is already empty, don't iterate through the loop a final time
        // Otherwise, another empty box would be added.
        if (empty($cross_to[$j]) AND ($j == ($num_crosses - 1))) {
          $num_crosses--;
        }
      }
      else { // This is a brand new select box, so make the default value empty.
        $cross_to_default = 0;
      }

      // Create the select box
      $form['parents'][$pid]['cross_to'][] = array(
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

  // NOTE: This can't be done via AJAX because we need to save parents to the
  // database & thus we NEED validation.
  $form['add-row'] = array(
    '#type' => 'submit',
    '#value' => 'Add Parent',
    '#prefix' => '<span class="cross-parents add-more">',
    '#suffix' => '</span>',
  );

  $form['submit-cb'] = array(
    '#type' => 'submit',
    '#value' => 'Submit Crossing Plans',
  );

  return $form;
}

/**
  * Form validation for breeders to set crosses.
  * Gives a warning when a cross is made with itself, 2+ identical crosses cannot occur
  */
function cross_manage_crossingblock_set_cross_parents_form_validate($form, &$form_state) {

  //dpm($form_state, 'VALIDATE: form state');

  // We are going to need to validate for duplicate line/source parents. In
  // order to avoid a nested loop situation we are going to keep track
  // of parent line/source combinations in the following array and then check
  // at the end for duplication.
  $stocksource_count = array();
  $stocksource_pid = array();

  // Look at each row in the same manner.
  foreach ($form_state['values']['parents'] as $pid => $row) {

    // VALIDATE: ensure that the line entered is valid and that the name
    // entered can be unique attached to a stock_id.
    // Note that this shouldn't be empty because it's marked required
    // but we are checking anyway since a database query is involved
    // and we all know there are sometimes #DrupalWTF moments ;).
    if (!empty($form_state['values']['parents'][$pid]['line'])) {

      $matching_stock_id = cross_manage_get_stock_by_name($form_state['values']['parents'][$pid]['line'], $form_state['build_info']['args'][0]);
      if (!$matching_stock_id) {
        form_set_error("parents][$pid][line", "No Germplasm exists with that unique name.");
      }
      elseif (is_numeric($matching_stock_id)) {
        $form_state['values']['parents'][$pid]['stock_id'] = $matching_stock_id;
      }
      else {
        $ids = array();
        foreach ($matching_stock_id as $ms_id) $ids[] = $ms_id->stock_id;
        $records = db_query('
          SELECT cs.nid, s.uniquename
          FROM {chado_stock} cs
          LEFT JOIN chado.stock s ON s.stock_id=cs.stock_id
          WHERE cs.stock_id IN (:ids)',
          array(':ids' => $ids));
        $links = array();
        foreach ($records as $r) {
          $links[] = l($r->uniquename, 'node/'.$r->nid);
        }
        $msg = t('The name, :name, is not unique (!links), try using the accession of the germplasm you meant.',
          array(':name' => $form_state['values']['parents'][$pid]['line'], '!links' => implode(', ', $links)));
        form_set_error("parents][$pid][line", $msg);
      }

      // Further prerparation for checking if two parents have the same
      // line/source. Here we are essentially flattening the form_state keeping
      // only the information we are interested in.
      if (!empty($form_state['values']['parents'][$pid]['source']) AND isset($form_state['values']['parents'][$pid]['stock_id'])) {
        $stocksource_key = $form_state['values']['parents'][$pid]['stock_id'] . '|' . $form_state['values']['parents'][$pid]['source'];
        $stocksource_count[$stocksource_key] = (isset($stocksource_count[$stocksource_key])) ? $stocksource_count[$stocksource_key]+1 : 1;
        $stocksource_pid[$pid] = $stocksource_key;
      }
    }

    // Specifically the "Cross To" elements...
    // We assume at this point that line is present and valid.
    if (isset($form_state['values']['parents'][$pid]['cross_to'])) {

      // VALIDATE: Ensure the user hasn't crossed the parent to itself.
      $duplicate_field_id = array_search($pid, $form_state['values']['parents'][$pid]['cross_to']);
      if (is_numeric($duplicate_field_id)) {

        // We only want to show the warning on ajax if the triggering element
        // is in the same row as the offending cross. The message becomes
        // confusing if shown in the wrong row.
        if (isset($form_state['triggering_element']['#ajax']) AND $form_state['triggering_element']['#parents'][1] == $pid) {
          $msg = t(
            'You have indicated you want to cross :pname with itself (See :index paternal parent). Please explain further in the "Instructions" box to the right.',
            array(
              ':pname' => $form_state['values']['parents'][$pid]['line'],
              ':pnum' => $form_state['values']['parents'][$pid]['pnum'],
              ':index' => cross_manage_format_number_ordinal($duplicate_field_id + 1)
            ));
          drupal_set_message($msg, 'warning');
        }
        // We always want to show it if a button was pressed.
        elseif (isset($form_state['clicked_button'])) {
          $msg = t(
            'P:pnum: You have indicated you want to cross :pname with itself (See :index paternal parent). Please explain further in the associated "Instructions" box.',
            array(
              ':pname' => $form_state['values']['parents'][$pid]['line'],
              ':pnum' => $form_state['values']['parents'][$pid]['pnum'],
              ':index' => cross_manage_format_number_ordinal($duplicate_field_id + 1)
            ));
          drupal_set_message($msg, 'warning');
        }
      }

      // VALIDATE: The user shouldn't make the sample cross 2X.
      // LOGIC: Since the values of the cross_to elements are parent numbers,
      // we can check for duplicate parent number by counting the occurances
      // of each value. If the array of counts is smaller than the array of
      // crosses then one of them must have been duplicated. Furthermore, we can
      // now search through the counts to find the duplicated parent number.
      $cross_count = array_count_values($form_state['values']['parents'][$pid]['cross_to']);
      if (sizeof($form_state['values']['parents'][$pid]['cross_to']) !== sizeof($cross_count)) {

        // Search through the counts for any duplicated parent number. Since the
        // parent number is the key we can use array_keys to get a list of
        // duplicated parent numbers.
        // We use array_filter with a callback to ensure we catch instances
        // where the user has selected the same parent 2+ times.
        $duplicated_pids = array_filter($cross_count, 'cross_manage_filter_keep_2plus', ARRAY_FILTER_USE_BOTH);

        // For each duplicated parent number, alert the user.
        foreach($duplicated_pids as $dup_pid => $count) {

          if ($dup_pid !== 0) {
            // Now we need a list of the element indices for the duplicated parent
            // number. We can use array_keys again on the cross_to elements
            // to find the indicies where the value is the duplicated parent number.
            $field_ids = array_keys($form_state['values']['parents'][$pid]['cross_to'], $dup_pid);


            // Set an error on the duplicate fields.
            $error_msg_1st = 'Duplicate Cross :maternal x :paternal.';
            $error_msg_rm = $error_msg_1st . ' Remove the :index paternal parent.';
            $error_tokens = array(
              ':maternal' => $form_state['values']['parents'][$pid]['line'],
              ':paternal' => $form_state['values']['parents'][$dup_pid]['line'],
              ':count' => $count,
              ':index' => 'duplicate',
            );

            $fi = 0;
            foreach($field_ids as $id) {
              $fi++;
              if ($fi == 1) {
                $message = t($error_msg_1st, $error_tokens);
              }
              else {
                $error_tokens[':index'] = cross_manage_format_number_ordinal($id + 1);
                $message = t($error_msg_rm, $error_tokens);
              }
              form_set_error('parents][' . $pid . '][cross_to][' . $id, $message);


              // For some reason we don't see form errors on AJAX but we do see
              // drupal messages. Therefore, warn the user on AJAX about the
              // duplication.
              if (isset($form_state['triggering_element']['#ajax'])) {
                drupal_set_message($message, 'warning');;
              }
            }
          }
        }

      }

    }
  }

  // VALIDATE: Ensure that two parents don't have the same stock_id & source
  // as this would cause a db (PDO) error on insert/update.
  // We use the custom stocksource_count array generated above to keep track of
  // the number of each stock/source combination making it easy to check once,
  // down here, how many duplications are present. This cannot be done within
  // the loop because we would get multiple messages if the combination
  // was present 3+ times.
  $dup_stocksources = array_filter($stocksource_count, 'cross_manage_filter_keep_2plus', ARRAY_FILTER_USE_BOTH);
  foreach ($dup_stocksources as $dup_combo => $count) {
    $dup_pids = array_keys($stocksource_pid, $dup_combo);
    foreach ($dup_pids as $pid) {
      form_set_error('parents]['.$pid.'][line',
        t(':name (P:pnum) has the same line and source as :count other parents.',
          array(':name' => $form_state['values']['parents'][$pid]['line'], ':pnum' => $form_state['values']['parents'][$pid]['pnum'],':count' => $count-1)));
    }
  }


  // VALIDATE: If the user clicked the "Add Parent" button then we need to ensure
  // the line/source are filled in but if a different button is pressed we need to
  // overlook the empty line/source.
  if ($form_state['clicked_button']['#value'] == 'Add Parent') {
    if (empty($form_state['values']['parents']['new']['line'])) {
      form_set_error('parents][new][line', 'The line is required to add a new parent.');
    }
    if (empty($form_state['values']['parents']['new']['source'])) {
      form_set_error('parents][new][source', 'The source is required to add a new parent.');
    }
  }
  // If BOTH are missing then we can just ignore the parent since it's not filled in.
  elseif (empty($form_state['values']['parents']['new']['line'])
        AND empty($form_state['values']['parents']['new']['source'])) {

      unset($form_state['values']['parents']['new']);
  }
  // Otherwise, maybe they didn't realize the line/source are required?
  else {
    if (empty($form_state['values']['parents']['new']['line'])) {
      form_set_error('parents][new][line', 'The line of the new parent needs to be entered before it can be saved. If you did not intend to add a new parent before submitting your crossing plans then delete it.');
    }
    if (empty($form_state['values']['parents']['new']['source'])) {
      form_set_error('parents][new][source', 'The source of the new parent needs to be entered before it can be saved. If you did not intend to add a new parent before submitting your crossing plans then delete it.');
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

  $form_state['rebuild'] = TRUE;

 //dpm($form_state, 'SUBMIT: Form State');

  // Regardless we want to save all the changes.
  foreach (element_children($form_state['values']['parents']) as $pid) {
    if ($pid != 'add-row') {

      // Table: cross_manage_parents.
      // Keeps track of basic details for each parent.
      $parent_record = array(
        'parent_number' => $form_state['values']['parents'][$pid]['pnum'],
        'stock_id' => $form_state['values']['parents'][$pid]['stock_id'],
        'cb_species' => $form_state['build_info']['args'][0],
        'cb_year' => $form_state['build_info']['args'][1],
        'cb_season' => $form_state['build_info']['args'][2],
        'seed_source' => $form_state['values']['parents'][$pid]['source']
      );

      // If there is no cb_parent_id then this is a new record.
      if (empty($form_state['values']['parents'][$pid]['cb_parent_id']) OR $pid == 'new') {
        drupal_write_record('cross_manage_parents', $parent_record);

        if (isset($parent_record['cb_parent_id'])) {
          $form_state['values']['parents'][$pid]['cb_parent_id'] = $parent_record['cb_parent_id'];
        }
        else {
          // ERROR: unable to save parent.
        }
      }
      else {
        $parent_record['cb_parent_id'] = $form_state['values']['parents'][$pid]['cb_parent_id'];
        drupal_write_record('cross_manage_parents', $parent_record, 'cb_parent_id');
      }

      if (isset($parent_record['cb_parent_id'])) {

        // Table: cross_manage_planned_crosses.
        // Keeps track of each cross planned. In other words stores each
        // maternal/paternal parent cross specified in the "Cross To" drop-downs.
        foreach ($form_state['values']['parents'][$pid]['cross_to'] as $pat_pid) {
          $cross_record = array();

          if ($pat_pid != 0) {
            $cross_record = array(
              'maternal_parent_id' => $parent_record['cb_parent_id'],
              'paternal_parent_id' => $pat_pid
            );

            $exists = db_query('
              SELECT cb_cross_id
              FROM {cross_manage_planned_crosses}
              WHERE maternal_parent_id=:mat AND paternal_parent_id=:pat',
              array(
                ':mat' => $cross_record['maternal_parent_id'],
                ':pat' => $cross_record['paternal_parent_id']
            ))->fetchField();
            if ($exists) {
              $cross_record['cb_cross_id'] = $exists;
              drupal_write_record('cross_manage_planned_crosses', $cross_record, 'cb_cross_id');
            }
            else {
              drupal_write_record('cross_manage_planned_crosses', $cross_record);
            }
          }
        }

        // Delete any removed planned crosses.
        // Logic: Restricted to the maternal_parent_id, delete any crosses
        // where the paternal parent is not in the cross_to list.
        if (sizeof($form_state['values']['parents'][$pid]['cross_to']) > 1) {
          db_query('DELETE FROM {cross_manage_planned_crosses} WHERE maternal_parent_id=:mat AND paternal_parent_id NOT IN (:pats)',
              array(':mat' => $parent_record['cb_parent_id'], ':pats' => $form_state['values']['parents'][$pid]['cross_to']))->fetchAll();
        }

        // Table: cross_manage_notes.
        // Keeps track of breeder's notes and instructions.
        if (!empty($form_state['values']['parents'][$pid]['notes']['breeder_notes'])) {
          $selfnote_record = array(
            'cb_parent_id' => $parent_record['cb_parent_id'],
            'type' => 'breeder_selfnote',
            'value' => $form_state['values']['parents'][$pid]['notes']['breeder_notes'],
          );

          $exists = db_query('
            SELECT true
            FROM {cross_manage_notes}
            WHERE cb_parent_id=:pid AND type=:type',
            array(
              ':pid' => $pid,
              ':type' => 'breeder_selfnote'
          ))->fetchField();
          if ($exists) {
            drupal_write_record('cross_manage_notes', $selfnote_record, array('cb_parent_id','type'));
          }
          else {
            drupal_write_record('cross_manage_notes', $selfnote_record);
          }
        }
        if (!empty($form_state['values']['parents'][$pid]['notes']['instructions'])) {
          $instruct_record = array(
            'cb_parent_id' => $parent_record['cb_parent_id'],
            'type' => 'fieldlab_instruction',
            'value' => $form_state['values']['parents'][$pid]['notes']['instructions']
          );

          $exists = db_query('
            SELECT true
            FROM {cross_manage_notes}
            WHERE cb_parent_id=:pid AND type=:type',
            array(
              ':pid' => $pid,
              ':type' => 'fieldlab_instruction'
          ))->fetchField();
          if ($exists) {
            drupal_write_record('cross_manage_notes', $instruct_record, array('cb_parent_id','type'));
          }
          else {
            drupal_write_record('cross_manage_notes', $instruct_record);
          }
        }
      }
    }
  }

  if ($form_state['clicked_button']['#value'] == 'Submit Crossing Plans') {
    $form_state['rebuild'] = FALSE;
    drupal_set_message('Successfully submitted crossing plans');
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
  // Determine the row for which AJACX was triggered.
  $pid = $form_state['triggering_element']['#array_parents'][1];

  // Save the current crosses.
  //dpm($form_state, 'AJAX: form state');
  foreach($form_state['values']['parents'][$pid]['cross_to'] as $paternal_pid) {
    if (!empty($pid) AND !empty($paternal_pid)) {
      $exists = db_query('SELECT true FROM {cross_manage_planned_crosses} WHERE maternal_parent_id=:mat AND paternal_parent_id=:pat',
        array(':mat' => $pid, ':pat' => $paternal_pid))->fetchField();
      if (!$exists) {
        $cross = array(
          'maternal_parent_id' => $pid,
          'paternal_parent_id' => $paternal_pid
        );
        drupal_write_record('cross_manage_planned_crosses', $cross);
      }
    }
  }

  // Remove any crosses with the current maternal parent that are not specified in the form.
  db_delete('cross_manage_planned_crosses')
    ->condition('maternal_parent_id', $pid)
    ->condition('paternal_parent_id',$form_state['values']['parents'][$pid]['cross_to'], 'NOT IN')
    ->execute();

  // Return the part of the form we want to render.
  // This is the cell in "Cross to" column for the row specified by $pid.
  return $form['parents'][$pid]['cross_to'];
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

  if (sizeof($form_state['build_info']['args']) >=3 ) {
    $cb_set = TRUE;
    $species = str_replace('-',' ',$form_state['build_info']['args'][0]);
    $cb_year = $form_state['build_info']['args'][1];
    $cb_season = $form_state['build_info']['args'][2];
  }
  else {
    drupal_set_message('Unable to determine the current crossing block', 'error');
    return $form;
  }

	// Set Breadcrumb:
	//--------------------------
	$breadcrumb = array();
	$breadcrumb[] = l('Home', '<front>');
	$breadcrumb[] = l('Crossing Block Management', 'crossing-block');
	$breadcrumb[] = l($cb_season. ' ' . $cb_year, "crossing-block/$species/$cb_year/$cb_season/summary");
	$breadcrumb[] = l('Plan Crosses: F1s', base_path() . request_uri()); // Link to current URL
	drupal_set_breadcrumb($breadcrumb);

  // Add Themeing:
  //--------------------------
  $crossmanage_path = drupal_get_path('module', 'cross_manage');
  $form['#attached'] = array(
    'css' => array(
      'type' => 'file',
      'data' => $crossmanage_path . '/theme/css/cross_manage_forms.css',
    ),
  );
  $form['#attached']['js'][] = $crossmanage_path . '/theme/js/CrossManage_highlightErrorRow.js';

  // So, the logic for retrieveing F1's is a little convoluted... Since part
  // of the data is stored in the cross_manage drupal tables and the rest is
  // stored in chado, we need to utilize cross schema joins. Plus, the first
  // time you load the F1 form it needs a complicated query to grab progeny
  // from the previous crossing block and add it as parents for this one.
  // Thus, first we need to check if there are any F1 parents for the
  // currect crossing block.
  $parents_exist = db_query("
    SELECT true
    FROM {cross_manage_parents}
    WHERE
      cb_year=:year AND
      cb_season=:season AND
      cb_species=:species AND
      parent_number='F1'",
    array(':year' => $cb_year, ':season' => $cb_season, ':species' => $species))->fetchField();

  // If there are not any F1 parents yet then we should add them before continuing.
  // This allows us to use the same query regarless of whether this is the
  // first time loading the F1 form for this crossing block or not.
  if (!$parents_exist) {
    cross_manage_add_f1s_as_parents($species, $cb_year, $cb_season);
  }

  // Now the F1's from the previous crossing block have already been added to
  // the current crossing block as parents so we can use a query very similar
  // to the one used for the parents form.
  $f1s = db_query("
    SELECT p.*, s.name, s.uniquename, cs.nid,
      type.name as type_name, num_seeds.value as num_seeds,
      self.value as breeder_notes, instruct.value as instructions,
      seedc.value as seed_cross, seedr.value as seed_reserve, seedg.value as seed_growout,
      (SELECT array_agg(paternal_parent_id) FROM cross_manage_planned_crosses WHERE maternal_parent_id=p.cb_parent_id) as cross_to
    FROM {cross_manage_parents} p
    LEFT JOIN chado.stock s ON s.stock_id=p.stock_id
    LEFT JOIN chado.cvterm type ON type.cvterm_id=s.type_id
    LEFT JOIN chado.stockprop num_seeds ON num_seeds.stock_id=s.stock_id AND num_seeds.type_id=:seeds_type_id
    LEFT JOIN {chado_stock} cs ON cs.stock_id = p.stock_id
    LEFT JOIN {cross_manage_notes} self ON self.cb_parent_id=p.cb_parent_id AND self.type='breeder_selfnote'
    LEFT JOIN {cross_manage_notes} instruct ON instruct.cb_parent_id=p.cb_parent_id AND instruct.type='fieldlab_instruction'
    LEFT JOIN {cross_manage_seed_allocation} seedc ON seedc.cb_parent_id=p.cb_parent_id AND seedc.type='cross'
    LEFT JOIN {cross_manage_seed_allocation} seedr ON seedr.cb_parent_id=p.cb_parent_id AND seedr.type='reserve'
    LEFT JOIN {cross_manage_seed_allocation} seedg ON seedg.cb_parent_id=p.cb_parent_id AND seedg.type='growout'
    WHERE cb_year=:year AND cb_season=:season AND cb_species=:species AND parent_number='F1'
    ORDER BY parent_number",
    array(
      'seeds_type_id' => '3973',
      ':year' => $cb_year,
      ':season' => $cb_season,
      ':species' => $species
  ))->fetchAll();

  // Set Page Title
  $species = str_replace('-',' ',$form_state['build_info']['args'][0]);
  $cb_year = $form_state['build_info']['args'][1];
  $cb_season = $form_state['build_info']['args'][2];
  drupal_set_title(format_string('Plan Crosses with F1s from @prev_cb', array(
      '@species' => $species,
      '@prev_cb' => $f1s[0]->seed_source,
  )));

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

  // Set the cross-to options - first value is blank so the drop-down is initially blank
  $parent_options = cross_manage_get_parent_options($species, $cb_year, $cb_season);

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
  // Titles are commented out to make things easier in the theme function
  // where these plain form fields will be rendered in a table layout.
  foreach ($f1s as $i => $parent) {

    // DEFAULTS:
    $has_form_state = (isset($form_state['values']['parents'])) ? TRUE : FALSE;
    $pid = $parent->cb_parent_id;

    $seed_cross = ($has_form_state) ? $form_state['values']['f1'][$pid]['seed_allocate']['cross'] : $parent->seed_cross;
    $seed_reserve = ($has_form_state) ? $form_state['values']['f1'][$pid]['seed_allocate']['reserve'] : $parent->seed_reserve;
    $seed_growout = ($has_form_state) ? $form_state['values']['f1'][$pid]['seed_allocate']['grow-out'] : $parent->seed_growout;

    $breeder_notes = ($has_form_state) ? $form_state['values']['f1'][$pid]['notes']['breeder_notes'] : $parent->breeder_notes;
    $instructions = ($has_form_state) ? $form_state['values']['f1'][$pid]['notes']['instructions'] : $parent->instructions;

    $cross_to = array();
    if (!empty($parent->cross_to)) {
      if (!is_array($parent->cross_to)) {
        $parent->cross_to = explode("," , trim($parent->cross_to, '{}'));
      }
      $cross_to = $parent->cross_to;
    }
    $cross_to = ($has_form_state AND $form_state['values']['f1'][$pid]['cross_to'][0] != 0) ? $form_state['values']['f1'][$pid]['cross_to'] : $cross_to;

    $cross_name = $parent->name;
    if (isset($f1s[$i]->nid)) {
      $cross_name = l($cross_name, 'node/'.$f1s[$i]->nid, array('attributes' => array('target' => '_blank')));
    }

    $form['f1'][$pid]['cross_num'] = array(
      '#type' => 'item',
      //'#title' => 'Cross Number',
      '#markup' => $cross_name,
    );

    // Same as cross_num, we also make it a hidden element to access it through
    // $form_state later.
    $form['f1'][$pid]['cross_name'] = array(
      '#type' => 'hidden',
      '#value' => $parent->name,
    );

    $form['f1'][$pid]['type'] = array(
      '#type' => 'item',
      //'#title' => 'Type',
      '#markup' => $parent->type_name,
    );

    // Check to see if we know the number of seeds produced from the previous crossing block
    // for this F1. If not, display a textbox to allow breeders to enter seed amount if they
    // know it (ie. OPTIONAL).
    if(isset($parent->num_seeds)) {
      $form['f1'][$pid]['seed_num'] = array(
        '#type' => 'item',
        //'#title' => 'Total Seed Produced',
        '#markup' => $parent->num_seeds,
      );
    }
    else {
      $form['f1'][$pid]['seed_num'] = array(
        '#type' => 'textfield',
        '#attributes' => array('placeholder' => 'Unknown'),
        '#size' => '7',
      );
    }

    $form['f1'][$pid]['seed_allocate']['cross'] = array(
      '#type' => 'textfield',
      '#title' => t("Cross:"),
      '#size' => 3,
      '#default_value' => $seed_cross,
    );

    $form['f1'][$pid]['seed_allocate']['reserve'] = array(
      '#type' => 'textfield',
      '#title' => t("Reserve:"),
      '#size' => 3,
      '#default_value' => $seed_reserve,
    );

    $form['f1'][$pid]['seed_allocate']['grow-out'] = array(
      '#type' => 'textfield',
      '#title' => t('Grow-out:'),
      '#size' => 3,
      '#default_value' => $seed_growout,
    );

    /** MARKERS
     * Add the ability to select markers to screen for each F1. Clicking on '+' would
     * bring up a popup window to allow breeders to browse/select markers.
     * @TODO: Implement marker selection
    /*
    $form['f1'][$pid]['marker_data']['markers2screen'] = array(
      '#type' => 'markup',
      //'#title' => 'Markers to Screen Already Selected',
      '#markup' => 'marker1, marker3, marker4'
        . '<img src="http://icons.iconarchive.com/icons/visualpharm/icons8-metro-style/256/Very-Basic-Plus-icon.png" height="16" weight="16" style="border:none;">'
        . ' '
        . '<img src="http://icons.iconarchive.com/icons/visualpharm/icons8-metro-style/256/Very-Basic-Minus-icon.png" height="16" weight="16" style="border:none;">'
    );
    */

    $form['f1'][$pid]['notes']['breeder_notes'] = array(
      '#type' => 'textarea',
      '#attributes' => array('placeholder' => 'Breeder Notes'),
      '#rows' => 1,
      '#default_value' => $breeder_notes
    );

    $form['f1'][$pid]['notes']['instructions'] = array(
      '#type' => 'textarea',
      '#attributes' => array('placeholder' => 'Instructions'),
      '#rows' => 1,
      '#default_value' => $instructions
    );

    // Set up a wrapper to allow ajax to replace this item
    $cross_to_wrapper = 'cross-to-' . $pid;
    $form['f1'][$pid]['cross_to'] = array(
      '#type' => 'item',
      '#prefix' => '<div id="' . $cross_to_wrapper . '">',
      '#suffix' => '</div>',
      //'#title' => 'Cross To',
    );

    // Add a number of cross to dropdowns to allow breeders to specify what
    // parents they want to cross with the current line
    // Ajax is used to ensure that there is always one more select box
    // than there are parents selected so that breeders can add more parents
    // to cross with.

    // Determine how many select boxes we need.
    // We don't need to add one because we are using the 0-index where
    // size starts at 1.
    $num_crosses = (!empty($cross_to)) ? sizeof($cross_to) : 0;

    for ($j = 0; $j <= $num_crosses; $j++) {
      // Determine our default value for the next select box.
      if (isset($cross_to[$j])) {
        // Default value is what has been previously entered
        $cross_to_default = $cross_to[$j];
        // If the final box is already empty, don't iterate through the loop a final time
        // Otherwise, another empty box would be added.
        if (empty($cross_to[$j]) AND ($j == ($num_crosses - 1))) {
          $num_crosses--;
        }
      }
      else { // This is a brand new select box, so make the default value empty.
        $cross_to_default = 0;
      }

      // Create the select box
      $form['f1'][$pid]['cross_to'][] = array(
        '#type' => 'select',
        '#options' => $parent_options,
        '#default_value' => $cross_to_default,
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

  return $form;
}

/**
 * Form validation for breeders to set crosses with F1s.
 * Gives a warning when a cross is made with itself, 2+ identical crosses cannot occur
 */
function cross_manage_crossingblock_set_cross_f1s_form_validate($form, &$form_state) {

  dpm($form_state['values']['f1'], "form_state");

  // We are going to need to validate for duplicate line/source parents. In
  // order to avoid a nested loop situation we are going to keep track
  // of parent line/source combinations in the following array and then check
  // at the end for duplication.
  $stocksource_count = array();
  $stocksource_pid = array();

  // Look at each row in the same manner
  foreach ($form_state['values']['f1'] as $pid => $row) {

    // Things to validate: cross_name with each cross_to, seed allocation is numerical & >0, notes

    if (isset($form_state['values']['f1'][$pid]['cross_to'])) {

      // VALIDATE: Ensure the user hasn't crossed the parent to itself.
      $duplicate_field_id = array_search($pid, $form_state['values']['f1'][$pid]['cross_to']);
      if (is_numeric($duplicate_field_id)) {

        // We only want to show the warning on ajax if the triggering element
        // is in the same row as the offending cross. The message becomes
        // confusing if shown in the wrong row.
        if (isset($form_state['triggering_element']['#ajax']) AND $form_state['triggering_element']['#parents'][1] == $pid) {
          $msg = t(
            'You have indicated you want to cross :pname with itself (See :index paternal parent). Please explain further in the "Instructions" box to the right.',
            array(
              ':pname' => $form_state['values']['f1'][$pid]['cross_name'],
              ':index' => cross_manage_format_number_ordinal($duplicate_field_id + 1)
            ));
          drupal_set_message($msg, 'warning');
        }
        // We always want to show it if a button was pressed.
        elseif (isset($form_state['clicked_button'])) {
          $msg = t(
            ':pname: You have indicated you want to cross :pname with itself (See :index paternal parent). Please explain further in the associated "Instructions" box.',
            array(
              ':pname' => $form_state['values']['f1'][$pid]['cross_name'],
              ':index' => cross_manage_format_number_ordinal($duplicate_field_id + 1)
            ));
          drupal_set_message($msg, 'warning');
        }
      }

      // VALIDATE: The user shouldn't make the sample cross 2X.
      // LOGIC: Since the values of the cross_to elements are cross numbers,
      // we can check for duplicate cross numbers by counting the occurances
      // of each value. If the array of counts is smaller than the array of
      // crosses then one of them must have been duplicated. Furthermore, we can
      // now search through the counts to find the duplicated cross number.
      $cross_count = array_count_values($form_state['values']['f1'][$pid]['cross_to']);
      //dpm($cross_count, "cross_count");

      if (sizeof($form_state['values']['f1'][$pid]['cross_to']) !== sizeof($cross_count)) {

        // Search through the counts for any duplicated cross number. Since the
        // cross number is the key we can use array_keys to get a list of
        // duplicated cross numbers.
        // We use array_filter with a callback to ensure we catch instances
        // where the user has selected the same parent 2+ times.
        $duplicated_pids = array_filter($cross_count, 'cross_manage_filter_keep_2plus', ARRAY_FILTER_USE_BOTH);

        //dpm($duplicated_pids, "dup_pids");

        // For each duplicated parent number, alert the user.
        foreach($duplicated_pids as $dup_pid => $count) {

          if ($dup_pid !== 0) {
            // Now we need a list of the element indices for the duplicated parent
            // number. We can use array_keys again on the cross_to elements
            // to find the indicies where the value is the duplicated parent number.
            $field_ids = array_keys($form_state['values']['f1'][$pid]['cross_to'], $dup_pid);

            // Set an error on the duplicate fields.
            $error_msg_1st = 'Duplicate Cross :maternal x :paternal.';
            $error_msg_rm = $error_msg_1st . ' Remove the :index paternal parent.';
            // Since paternal parent might be from the Parents form, we have to check
            // the name in the database.
            $paternal = cross_manage_get_parent(array('cb_parent_id' => $dup_pid));
            $error_tokens = array(
              ':maternal' => $form_state['values']['f1'][$pid]['cross_name'],
              ':paternal' => $paternal->stock->name,
              ':count' => $count,
              ':index' => 'duplicate',
            );

            $fi = 0;
            foreach($field_ids as $id) {
              $fi++;
              if ($fi == 1) {
                $message = t($error_msg_1st, $error_tokens);
              }
              else {
                $error_tokens[':index'] = cross_manage_format_number_ordinal($id + 1);
                $message = t($error_msg_rm, $error_tokens);
              }
              form_set_error('f1][' . $pid . '][cross_to][' . $id, $message);

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

  $form_state['rebuild'] = TRUE;

  // Iterate through each f1 since we want to save all changes
  foreach (element_children($form_state['values']['f1']) as $pid) {

    // Table: cross_manage_parents
    // Keep track of simple form elements for each cross - we don't need cross name or type
    // since these cannot be changed in the form
    $f1_record = array(
      'cb_parent_id' => $pid,
      'seed_num' => $form_state['values']['f1'][$pid]['seed_num'],
      'seedc' => $form_state['values']['f1'][$pid]['seed_allocate']['cross'],
      'seedr' => $form_state['values']['f1'][$pid]['seed_allocate']['reserve'],
      'seedg' => $form_state['values']['f1'][$pid]['seed_allocate']['grow-out'],
    );

    // Table: cross_manage_planned_crosses
    // Keeps track of each cross that is planned, ie. stores each maternal/paternal parent
    // cross specified in the "Cross To" drop-downs.
    foreach ($form_state['values']['f1'][$pid]['cross_to'] as $pat_pid) {
      $cross_record = array();

      if ($pat_pid != 0) {
        $cross_record = array(
          'maternal_parent_id' => $f1_record['cb_parent_id'],
          'paternal_parent_id' => $pat_pid,
        );

        $exists = db_query('
          SELECT cb_cross_id
          FROM {cross_manage_planned_crosses}
          WHERE maternal_parent_id=:mat AND paternal_parent_id=:pat',
          array(
            ':mat' => $cross_record['maternal_parent_id'],
            ':pat' => $cross_record['paternal_parent_id']
        ))->fetchField();
        if ($exists) {
          $cross_record['cb_cross_id'] = $exists;
          drupal_write_record('cross_manage_planned_crosses', $cross_record, 'cb_cross_id');
        }
        else {
          drupal_write_record('cross_manage_planned_crosses', $cross_record);
        }
      }
    }

    // Delete any removed planned crosses.
    // Logic: Restricted to the maternal_parent_id, delete any crosses
    // where the paternal parent is not in the cross_to list.
    if (sizeof($form_state['values']['f1'][$pid]['cross_to']) > 1) {
      db_query('DELETE FROM {cross_manage_planned_crosses} WHERE maternal_parent_id=:mat AND paternal_parent_id NOT IN (:pats)',
          array(':mat' => $f1_record['cb_parent_id'], ':pats' => $form_state['values']['f1'][$pid]['cross_to']))->fetchAll();
    }

    // Table: cross_manage_notes.
    // Keeps track of breeder's notes and instructions.
    if (!empty($form_state['values']['f1'][$pid]['notes']['breeder_notes'])) {
      $selfnote_record = array(
        'cb_parent_id' => $f1_record['cb_parent_id'],
        'type' => 'breeder_selfnote',
        'value' => $form_state['values']['f1'][$pid]['notes']['breeder_notes'],
      );
      $exists = db_query('
        SELECT true
        FROM {cross_manage_notes}
        WHERE cb_parent_id=:pid AND type=:type',
        array(
          ':pid' => $pid,
          ':type' => 'breeder_selfnote'
      ))->fetchField();
      if ($exists) {
        drupal_write_record('cross_manage_notes', $selfnote_record, array('cb_parent_id','type'));
      }
      else {
        drupal_write_record('cross_manage_notes', $selfnote_record);
      }
    }
    if (!empty($form_state['values']['f1'][$pid]['notes']['instructions'])) {
      $instruct_record = array(
        'cb_parent_id' => $f1_record['cb_parent_id'],
        'type' => 'fieldlab_instruction',
        'value' => $form_state['values']['f1'][$pid]['notes']['instructions']
      );

      $exists = db_query('
        SELECT true
        FROM {cross_manage_notes}
        WHERE cb_parent_id=:pid AND type=:type',
        array(
          ':pid' => $pid,
          ':type' => 'fieldlab_instruction'
      ))->fetchField();
      if ($exists) {
        drupal_write_record('cross_manage_notes', $instruct_record, array('cb_parent_id','type'));
      }
      else {
        drupal_write_record('cross_manage_notes', $instruct_record);
      }
    }
  }

  if ($form_state['clicked_button']['#value'] == 'Send to Field Lab') {
    $form_state['rebuild'] = FALSE;
    drupal_set_message('Successfully submitted F1s form to Field Lab');
  }
}

/**
 * Callback for ajax-enabled select boxes
 * Retrieves the cross_num from the row that was "triggered" and returns the cross-to section
 * of the form for that row.
 */
function ajax_f1_cross_to_callback($form, $form_state) {
  $cross_num = $form_state['triggering_element']['#array_parents'][1];
  return $form['f1'][$cross_num]['cross_to'];
  dpm($form, "form");
}

/**
 * The first time the F1/parents form is visisted for a given crossing block,
 * the F1's from the previous crossing block need to be added to this one.
 */
function cross_manage_add_f1s_as_parents($species, $cb_year, $cb_season) {

    // Get the year/season for the PREVIOUS corssing block since that's
    // where the record for the F1's would be.
    // To determine previous crossing block, we need to choose the previous
    // season and if the current season is spring then choose winter of the
    // previous year.
    $seasons = array('Spring', 'Summer', 'Fall', 'Winter');
    $curr_season_key = array_search($cb_season, $seasons);
    if ($curr_season_key > 0) {
      $prev_cb_season = $seasons[$curr_season_key - 1];
      $prev_cb_year = $cb_year;
    } else {
      $prev_cb_season = 'Winter';
      $prev_cb_year = $cb_year - 1;
    }

    // The following query pulls all F1s from the PREVIOUS crossing block.
    // Where this becomes difficult is that the breeders don't put
    // in a crossing block every quarter so if the query below returns 0 then
    // you need to grab for those for 2 crossing blocks ago.
    $f1s = chado_query("
      SELECT
        s.stock_id
      FROM {stock} s
      LEFT JOIN {stockprop} cby ON s.stock_id=cby.stock_id AND cby.type_id=:cb_year_id
      LEFT JOIN {stockprop} cbs ON s.stock_id=cbs.stock_id AND cbs.type_id=:cb_season_id
      LEFT JOIN {organism} org ON org.organism_id=s.organism_id
      WHERE cby.value=:cb_year AND cbs.value=:cb_season AND org.genus=:genus
      ORDER BY s.name ASC",
      array(
        'cb_year_id' => '3978',
        'cb_season_id' => '3977',
        'genus' => $species,
        'cb_year' => $prev_cb_year,
        'cb_season' => $prev_cb_season,
      )
    );
    $num_f1s = 0;
    foreach ($f1s as $r) {
      $num_f1s++;
      $parent_record = array(
        'parent_number' => 'F1',
        'stock_id' => $r->stock_id,
        'cb_species' => $species,
        'cb_year' => $cb_year,
        'cb_season' => $cb_season,
        'seed_source' => $prev_cb_season.' '.$prev_cb_year.' Crossing Block'
      );
      drupal_write_record('cross_manage_parents', $parent_record);
    }

  // @TODO: Implement recursion to travel to previous*2+ if the last season did
  // not have a crossing block.
  // This is not as simple as simply calling the current function with the prev
  // values for crossing block since we need the progeny to be associated
  // with this crossing block not the last one..
}
