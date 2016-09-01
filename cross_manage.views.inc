<?php

/**
 * @file
 * Contains views integration for lims.
 */

/**
 * Implements hook_views_data().
 */
function cross_manage_views_data() {
  $data = array();

  // Cross Management: Parents
  //---------------------------------------------  
  $data['cross_manage_parents']['table']['group'] = t('Cross Parents');
  $data ['cross_manage_parents']['table']['base'] = array(
    'field' => 'cb_parent_id', // This is the identifier field for the view.
    'title' => t('Cross Parents'),
    'help' => t('Parents of a given crossing block.'),
    'weight' => -10,
  );

  $data ['cross_manage_parents']['table']['join'] = array(
    // Index this array by the table name to which this table refers.
    // 'left_field' is the primary key in the referenced table.
    // 'field' is the foreign key in this table.
    'stock' => array(
      'left_field' => 'stock_id',
      'field' => 'stock_id',
    ),
  );

  // cb_parent_id
  $data['cross_manage_parents']['cb_parent_id'] = array(
    'title' => t('Parent ID'),
    'help' => t('The primary identifier for a crossing block parent.'),
  );

  // stock_id
  $data['cross_manage_parents']['stock_id'] = array(
    'title' => t('Germplasm content'),
    'help' => t('The germplasm information for a parent.'),
    // Define a relationship to the {node} table, so example_table views can
    // add a relationship to nodes. If you want to define a relationship the
    // other direction, use hook_views_data_alter(), or use the 'implicit' join
    // method described above.
    'relationship' => array(
      'base' => 'stock', // The name of the table to join with.
      'base field' => 'stock_id', // The name of the field on the joined table.
      // 'field' => 'nid' -- see hook_views_data_alter(); not needed here.
      'handler' => 'views_handler_relationship',
      'label' => t('Chado Stock'),
      'title' => t('Chado Stock'),
      'help' => t('Join the stock content to the crossing block parent.'),
    ),
  );
  
  // parent_number
  $data ['cross_manage_parents']['parent_number'] = array(
    'title' => t('Parent Number'),
    'help' => t('The number identifying the parent of the crossing block.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );
  
  // cb_species
  $data ['cross_manage_parents']['cb_species'] = array(
    'title' => t('Crossing Block Genus'),
    'help' => t('The genus of the parents used for the crossing block.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );
    
  // cb_year
  $data['cross_manage_parents']['cb_year'] = array(
    'title' => t('Crossing Block Year'),
    'help' => t('The year the crossing block was started in.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument',
    ),
  );
  
  // cb_season
  $data ['cross_manage_parents']['cb_season'] = array(
    'title' => t('Crossing Block Season'),
    'help' => t('The season the crossing block was started in.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );
  
  // seed_source
  $data ['cross_manage_parents']['seed_source'] = array(
    'title' => t('Seed Source'),
    'help' => t('The source of the seed used to grow the parent for crossing.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // Cross Management: Crosses
  //---------------------------------------------  
  $data['cross_manage_planned_crosses']['table']['group'] = t('Planned Crosses');
  $data ['cross_manage_planned_crosses']['table']['base'] = array(
    'field' => 'cb_cross_id', // This is the identifier field for the view.
    'title' => t('Planned Crosses'),
    'help' => t('The crosses planned to be completed in a given crossing block.'),
    'weight' => -10,
  );

  // cb_cross_id
  $data['cross_manage_planned_crosses']['cb_cross_id'] = array(
    'title' => t('Cross ID'),
    'help' => t('The primary identifier for a crossing block planned cross.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
  );

  // maternal_parent_id
  $data['cross_manage_planned_crosses']['maternal_parent_id'] = array(
    'title' => t('Maternal Parent'),
    'help' => t('The maternal parent of the planned cross.'),
    // Define a relationship to the {node} table, so example_table views can
    // add a relationship to nodes. If you want to define a relationship the
    // other direction, use hook_views_data_alter(), or use the 'implicit' join
    // method described above.
    'relationship' => array(
      'base' => 'cross_manage_parents', // The name of the table to join with.
      'base field' => 'cb_parent_id', // The name of the field on the joined table.
      'handler' => 'views_handler_relationship',
      'label' => t('Maternal Parent'),
      'title' => t('Maternal Parent'),
      'help' => t('The maternal parent of the planned cross.'),
    ),
  );

  // paternal_parent_id
  $data['cross_manage_planned_crosses']['paternal_parent_id'] = array(
    'title' => t('Paternal Parent'),
    'help' => t('The paternal parent of the planned cross.'),
    // Define a relationship to the {node} table, so example_table views can
    // add a relationship to nodes. If you want to define a relationship the
    // other direction, use hook_views_data_alter(), or use the 'implicit' join
    // method described above.
    'relationship' => array(
      'base' => 'cross_manage_parents', // The name of the table to join with.
      'base field' => 'cb_parent_id', // The name of the field on the joined table.
      'handler' => 'views_handler_relationship',
      'label' => t('Paternal Parent'),
      'title' => t('Paternal Parent'),
      'help' => t('The paternal parent of the planned cross.'),
    ),
  );

  // cross_stock_id
  $data['cross_manage_planned_crosses']['cross_stock_id'] = array(
    'title' => t('Resulting Stock'),
    'help' => t('The resulting stock denoting the cross.'),
    // Define a relationship to the {node} table, so example_table views can
    // add a relationship to nodes. If you want to define a relationship the
    // other direction, use hook_views_data_alter(), or use the 'implicit' join
    // method described above.
    'relationship' => array(
      'base' => 'stock', // The name of the table to join with.
      'base field' => 'stock_id', // The name of the field on the joined table.
      'handler' => 'views_handler_relationship',
      'label' => t('Resulting Stock'),
      'title' => t('Resulting Stock'),
      'help' => t('The resulting stock denoting the cross.'),
    ),
  );
  
  
  return $data;
}