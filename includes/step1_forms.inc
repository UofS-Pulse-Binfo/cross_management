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
function cross_manage_crossingblock_set_cross_parents_form($form, $form_state) {

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
  $form['crossingblock'] = array(
    '#type' => 'fieldset',
    '#title' => 'Crossing Block',
    '#collapsible' => ($cb_set) ? TRUE: FALSE,
    '#collapsed' => ($cb_set) ? TRUE: FALSE,
  );

  $form['crossingblock']['crop'] = array(
    '#type' => 'select',
    '#title' => 'Crop',
    '#description' => 'The crop you are designing the crossing block for.',
    '#options' => array(
      'Cicer arietinum' => 'Chickpea',
      'Phaseolus vulgaris' => 'Common Bean',
      'Vicia faba' => 'Faba Bean',
      'Pisum sativum' => 'Field Pea',
      'Lens culinaris' => 'Lentil',
    ),
    '#default_value' => $species,
  );

  $form['crossingblock']['year'] = array(
    '#type' => 'select',
    '#title' => 'Year',
    '#description' => 'The year the crosses should be made in.',
    '#options' => array_combine(range(date("Y")+2, 2000), range(date("Y")+2, 2000)),
    '#default_value' => $cb_year,
  );

  $form['crossingblock']['season'] = array(
    '#type' => 'select',
    '#title' => 'Season',
    '#description' => 'The season the crosses should be made in.',
    '#options' => array('Spring' => 'Spring','Summer' => 'Summer', 'Fall' => 'Fall', 'Winter' => 'Winter'),
    '#default_value' => $cb_season,
  );

  $form['crossingblock']['continue'] = array(
    '#type' => ($cb_set) ? 'hidden': 'submit',
    '#value' => 'Continue',
  );

  if ($cb_set) {

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

    $form['parents'] = array(
      '#type' => ($cb_set) ? 'fieldset' : 'hidden',
      '#title' => 'Parents',
      '#markup' => 'Please indicate the parents of the crosses you want to be made in the current crossing block',
      '#tree' => TRUE,
      '#theme' => 'cross_manage_crossingblock_parents'

    );

    $parent_options = array(0 => '', 1 => 'P1', 2 => 'P2', 3 => 'P3');

    $mock_options = array();
    $mock_options[] = array(
      'pnum' => 1,
      'line' => "CDC Redberry",
      'line_nid' => 971836,
      'source' => "Breeder's Seed",
      'cross_to' => array(1 => 2, 2 => 0),
      'details' => array(
        'market_class' => array('title' => 'Market Class', 'value' => 'Small Red'),
        'PEDIGREE' => array('title' => 'Pedigree', 'value' => 'Uknown/819-5R'),
        'ORIGINAL_CROSS' => array('title' => 'Original Cross Number', 'value' => l('1254S-1','node/972302')),
        'CROSSING_BLOCK' => array('title' => 'Parent in Recent Crossing Blocks', 'value' => 'Winter 2010 (P15), Winter 2009 (P7), Fall 2009 (P27), Summer 2009 (P27)'),
      ),
      'markers' => array(
        'marker1' => array(
          'name' => 'marker1',
          'marker_type' => 'KASP',
          'locus_type' => 'SNP',
          'allele' => 'A'
        ),
        'marker2' => array(
          'name' => 'marker2',
          'marker_type' => 'KASP',
          'locus_type' => 'SNP',
          'allele' => 'c'
        ),
        'marker3' => array(
          'name' => 'marker3',
          'marker_type' => 'Illumina GoldenGate',
          'locus_type' => 'SNP',
          'allele' => 'GG'
        ),
        'marker4' => array(
          'name' => 'marker4',
          'marker_type' => 'SSR',
          'locus_type' => 'SSR',
          'allele' => '(ATA)3 TTA (ATA)3'
        ),
        'marker5' => array(
          'name' => 'marker5',
          'marker_type' => 'KASP',
          'locus_type' => 'SNP',
          'allele' => 'C'
        ),
      ),
    );
    $mock_options[] = array(
      'pnum' => 2,
      'line' => "6382S",
      'line_nid' => 977354,
      'source' => "Previous Crossing Block",
      'cross_to' => array(1 => 3, 2 => 1, 3 => 0),
      'details' => array(
        'market_class' => array('title' => 'Market Class', 'value' => 'French Green'),
        'PEDIGREE' => array('title' => 'Pedigree', 'value' => l('3160-21','node/975259')."/".l('4540-1','node/977335')),
        'CROSSING_BLOCK' => array('title' => 'Created in', 'value' => 'Winter 2012'),
      ),
      'markers' => array(
        'marker1' => array(
          'name' => 'marker1',
          'marker_type' => 'KASP',
          'locus_type' => 'SNP',
          'allele' => 'T'
        ),
        'marker2' => array(
          'name' => 'marker2',
          'marker_type' => 'KASP',
          'locus_type' => 'SNP',
          'allele' => 'G'
        ),
        'marker3' => array(
          'name' => 'marker3',
          'marker_type' => 'Illumina GoldenGate',
          'locus_type' => 'SNP',
          'allele' => 'GG'
        ),
        'marker4' => array(
          'name' => 'marker4',
          'marker_type' => 'SSR',
          'locus_type' => 'SSR',
          'allele' => '(ATA)5 ATT (ATA)1'
        ),
        'marker5' => array(
          'name' => 'marker5',
          'marker_type' => 'KASP',
          'locus_type' => 'SNP',
          'allele' => 'C'
        ),
      ),
    );
    $mock_options[] = array(
      'pnum' => 3,
      'line' => "L01",
      'source' => "",
      'cross_to' => array(1 => 0),
      'details' => array(),
      'line_not_valid' => TRUE
    );


    $tmp = array_keys($mock_options[0]['markers']);
    $marker_options = array_combine($tmp,$tmp);
    $marker_options[0] = 'Select Marker';

    foreach ($mock_options as $k => $opt) {
      $pnum = $opt['pnum'];
      $line = $opt['line'];
      $source = $opt['source'];
      $cross_to = $opt['cross_to'];
      $details = $opt['details'];
      $line_not_valid = (isset($opt['line_not_valid'])) ? $opt['line_not_valid']: FALSE;
      $line_validitity = ($line_not_valid) ? 'line-invalid' : 'line-valid';

      $highlight_class = 'no-highlight';
      if (isset($opt['details'][$highlight_detail])) {
        $highlight_class = str_replace(' ','-', $opt['details'][$highlight_detail]['value']);
      }

      $form['parents'][$pnum] = array(
        '#type' => 'markup',
        //'#prefix' => '<span class="cross-parents row '.$highlight_class.'">',
        //'#suffix' => '</span>'
      );
      if ($k+1 == sizeof($mock_options)) {
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

      // Add in Marker Data or Selection
      $form['parents'][$pnum]['marker_data'] = array(
        '#type' => 'markup',
      );

      if ($marker_data_avail) {

      }
      else {
        $form['parents'][$pnum]['marker_data']['markers2screen'] = array(
          '#type' => 'markup',
          //'#title' => 'Markers to Screen Already Selected',
          '#markup' => 'marker1, marker3, marker4'
            . '<img src="http://icons.iconarchive.com/icons/visualpharm/icons8-metro-style/256/Very-Basic-Plus-icon.png">'
            . ' '
            . '<img src="http://icons.iconarchive.com/icons/visualpharm/icons8-metro-style/256/Very-Basic-Minus-icon.png">'
        );
        $form['parents'][$pnum]['marker_data']['is_data'] = array(
          '#type' => 'markup',
          '#value' => FALSE
        );
      }

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

      $form['parents'][$pnum]['cross_to'] = array(
        '#type' => 'item',
        //'#title' => 'Cross To',
      );

      foreach ($cross_to as $cross_to_num => $cross_to_default) {
        $form['parents'][$pnum]['cross_to'][$cross_to_num] = array(
          '#type' => 'select',
          '#options' => $parent_options,
          '#default_value' => $cross_to_default
        );
      }
    }

    $form['parents']['add-row'] = array(
      '#type' => 'submit',
      '#value' => 'Add Parent',
      '#prefix' => '<span class="cross-parents add-more">',
      '#suffix' => '</span>'
    );

    $form['submit-cb'] = array(
      '#type' => 'submit',
      '#value' => 'Submit Crossing Plans'
    );
  }

  return $form;
}

/**
 * MOCK-UP: Form allowing breeders to set the crosses to be made
 * for a given crossing block.
 *
 * Implements hook_form_submit().
 */
function cross_manage_crossingblock_set_cross_parents_form_submit(&$form, &$form_state) {

}

/**
 * Implements hook_form().
 * Form: Plan Crosses: F1's
 *
 * Form allowing breeders to indicate which germplasm should be crossed to the
 * F1's resulting from the previous crossing block. The ability to provide
 * additional instructions including which F1s to grow-out is also available.
 */
function cross_manage_crossingblock_set_cross_f1s_form($form, $form_state) {

  // My Hardcoded F1 data for the example.
  // @TODO: Select this data from chado for the crossing block previous to the current one.
  $mock = array(
    '2604S' => array('2604S','SX','P1/P15','NY5-161 White/Jetwhite4-11-21', array('2605S','P6','P7')),
    '2605S' => array('2605S','SX','P1/P22','NY5-161 White/WC-BC1-1',array('2604S')),
    '2606S' => array('2606S','SX','P2/P16','NY5-161 Brown/Jetblack4-15-3',array('2608S','P4')),
    '2607aS' => array('2607aS','SX','P2/P22','NY5-161 Brown/WC-BC1-1',array('P5')),
    '2608S' => array('2608S','SX','P3/P17','G9345/Jetblack4-11-11',array()),
    '2610bS' => array('2610bS','SX','P24/P11','1533a-15/Higuera-E',array()),
    '2613bS' => array('2613bS','SX','P47/P13','GDR6aa/737-22',array('P12','P13','P15','P17')),
    '2615S' => array('2615S','SX','P20/P15','316-13/Jetwhite4-11-21',array('P31')),
    '2616S' => array('2616S','SX','P26/P15','1441-8/Jetwhite4-11-21',array('P4')),
    '2617S' => array('2617S','SX','P27/P15','Expresso-5/Jetwhite4-11-21',array('P5')),
    '2618S' => array('2618S','SX','P28/P15','1780TF3-1/Jetwhite4-11-21',array('P6','P7')),
    '2619S' => array('2619S','SX','P41/P15','GDR32a-2/Jetwhite4-11-21',array('P12','P13')),
    '2620bS' => array('2620bS','SX','P20/P16','316-13/Jetblack4-15-3',array('P31')),
    '2621S' => array('2621S','SX','P26/P16','1441-8/Jetblack4-15-3',array('2622S')),
    '2622S' => array('2622S','SX','P27/P16','Expresso-5/Jetblack4-15-3',array('P4','P5'))
  );

  // My Hardcoded Parents for the example.
  // These parents were added for this crossing block using the parents form
  // defined in cross_manage_crossingblock_set_cross_parents_form().
  // @TODO: Select these from the chado stock table.
  $parent_options = array('2604S', '2608S', '2622S', 'P4','P5','P6','P7','P12','P13','P15','P17','P31');
  $parent_options = array_combine($parent_options, $parent_options);

  $form['f1'] = array(
    '#type' => 'fieldset',
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
      '#markup' => $details[2] . '<br />' . $details[3],
    );

    $form['f1'][$cross_num]['marker_data']['markers2screen'] = array(
      '#type' => 'markup',
      //'#title' => 'Markers to Screen Already Selected',
      '#markup' => 'marker1, marker3, marker4'
        . '<img src="http://icons.iconarchive.com/icons/visualpharm/icons8-metro-style/256/Very-Basic-Plus-icon.png" height="16" weight="16" style="border:none;">'
        . ' '
        . '<img src="http://icons.iconarchive.com/icons/visualpharm/icons8-metro-style/256/Very-Basic-Minus-icon.png" height="16" weight="16" style="border:none;">'
    );

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

    $form['f1'][$cross_num]['notes']['grow_out'] = array(
      '#type' => 'checkbox',
      '#title' => 'Grow-out?',
      '#default_value' => TRUE,
    );

    $form['f1'][$cross_num]['cross_to'] = array(
      '#type' => 'item',
      //'#title' => 'Cross To',
    );

    // Add a number of cross to dropdowns to allow breeders to specify what
    // parents they want to cross with the current F1
    // NOTE: Ajax is used to ensure that there is always one more select box
    // than there are parents selected to ensure that breeders can add more parents.
    // @TODO: Implement ajax.
    foreach ($details[4] as $cross_to_num => $cross_to_default) {
      $form['f1'][$cross_num]['cross_to'][$cross_to_default] = array(
        '#type' => 'select',
        '#options' => $parent_options,
        '#default_value' => $cross_to_default
      );
    }
  }

  return $form;
}

/**
 * MOCK-UP Form allowing breeders to set the crosses to be made
 * for a given crossing block.
 *
 * Implements hook_form_submit().
 */
function cross_manage_crossingblock_set_cross_f1s_form_submit($form, $form_state) {

}
