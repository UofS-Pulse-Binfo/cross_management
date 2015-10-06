<?php

  // Set the title to the particular crossing block.
  $title = format_string('!species: !season !year Crossing Block',
    array('!species' => str_replace('-',' ',$crossingblock_species),
      '!season' => $crossingblock_season,
      '!year' => $crossingblock_year));
  drupal_set_title($title);

  // Set the breadcrumb.
  $breadcrumb = array();
	$breadcrumb[] = l('Home', '<front>');
	$breadcrumb[] = l('Crossing Block Management', 'crossing-block');
	$breadcrumb[] = l($crossingblock_season. ' ' . $crossingblock_year, base_path() . request_uri()); // Link to current URL
	drupal_set_breadcrumb($breadcrumb);
	
  // Checkmark image.
  $checkmark_img_html = '<img src="http://images.sodahead.com/polls/004121505/3412401521_transparent_green_checkmark_md_answer_1_xlarge.png" width=20 height=20 style="border:none;">';
?>

<script>
  $(function() {
    $( "#accordion" ).accordion({
      collapsible: true,
      active: <?php print $form_steps['active_panel']; ?>,
      heightStyle: "content"
    });
  });
</script>

<div id="accordion" class="workflow-diagram">
  <h3 class="step breeder">
  <?php if ($form_steps['step1']['complete']) { print $checkmark_img_html; } ?>
  Plan Crosses by selecting Parents and F1's
  <span class="header-icons">

<style>
#step-nav-icons {
  margin-top: 50px;
}
#step-nav-icons ul {
  list-style-type: none;
  text-align: center;
  list-style-position: inside;
  padding-left: 0px;
}
#step-nav-icons ul li {
  float: left;
  width: 150px;
  height: 100px;
  margin: 40px 20px;
  margin-top: 0px;
}
#step-nav-icons .title{
  font-weight: bold;
  text-align: center;
  border-bottom: 1px solid #d0d0d0;
  padding-bottom: 4px;
}
#step-nav-icons .description {
  font-size: 80%;
  padding-top: 8px;
}
#step-nav-icons li {
  background-position: center top;
  background-repeat: no-repeat;
  background-size: 30%;
  padding-top: 50px;
}
#step-nav-icons li.form {
  background-image: url('data:image/png;base64,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');
}
#step-nav-icons li.list {
  background-image: url('data:image/png;base64,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');
</style>

  </span>
  </h3>
  <div class="step-content">
    <p class="description">Each season the crossing block is started by
      the Breeder's who select which of the F1's from the previous crossing
      block should re-enter the crossing block, as well as, additional
      germplasm to be used as parents in the new crosses.</p>
    <div id="step-nav-icons" class="step-1">
      <ul>
        <li class="form"><div class="title"><?php print l("Set Parents", $form_paths['parents']); ?></div>
          <div class="description">Indicate which germplasm will be parents for the upcoming crosses.</div></li>
        <li class="form"><div class="title"><?php print l("F1's Form", $form_paths['F1']); ?></div>
          <div class="description">Carry forward F1's from the previous crossing block.</div></li>
        <li class="list"><div class="title"><?php print l("Full Parent List", $list_paths['parent_list']); ?></div>
          <div class="description">List all parents (including F1s) for the current crossing block.</div></li>
      </ul>
    </div>
  </div>

<!--
  <h3 class="step marker-screen">
    <?php if ($form_steps['step2']['complete']) { print $checkmark_img_html; } ?>
    Screen the Seed for Markers
  </h3>
  <div class="step-content">
    <p class="description"> Seed needed for planned crosses will ideally be screened
      for the markers specified in this step, after which only seeds containing the
      markers of interest will be planted. Screening for markers can occur
      at any point prior to making the crosses, however, thus allowing the breeder to
      choose plants involved in crosses after planting.</p>
    <div id="step-nav-icons" class="step-2">
      <ul>
        <li class="form"><div class="title"><?php print l("Select Markers", $form_paths['parents']); ?></div>
          <div class="description">Indicate which markers each germplasm should be screened for.</div></li>
        <li class="form"><div class="title"><?php print l("Enter Marker Results", $form_paths['marker']); ?></div>
          <div class="description">Enter marker allele calls for each germplasm.</div></li>
        <li class="list"><div class="title">Marker Screening</div>
          <div class="description">Lists all the germplasm and the markers each should be screened with.</div></li>
        <li class="list"><div class="title">Marker Data</div>
          <div class="description">Lists all the marker data currently entered in a seed set by marker grid.</div></li>
      </ul>
    </div>
  </div>
-->

  <h3 class="step field-lab">
    <?php if ($form_steps['step3']['complete']) { print $checkmark_img_html; } ?>
    Plant the Crossing Block
  </h3>
  <div class="step-content">
    <p class="description">Members of the field lab will collect all
      the seed needed for the current crossing block based on sources
      specified by the breeders in the first step, and on seed from those
      sources that have been screened for markers. Once the specific seeds
      have been chosen for the crossing block, they are planted in the field
      or greenhouse.</p>
    <div id="step-nav-icons" class="step-3">
      <ul>
        <li class="list"><div class="title"><?php print l("Seed List", $list_paths['seed_list']); ?></div>
          <div class="description">Lists all the seed required for the current crossing block.</div></li>
        <!--<li class="list"><div class="title"><?php print l("Labels Template", $list_paths['parent_labels']); ?></div>
          <div class="description">Template for labels to tag parents in the field.</div></li>-->
        <li class="list"><div class="title"><?php print l("Crosses", $list_paths['crosses_basic']); ?></div>
          <div class="description">Lists the crosses to be done for this crossing block.</div></li>
        <li class="list"><div class="title"><?php print l("Grow-outs", $list_paths['growouts']); ?></div>
          <div class="description">Lists the number of F1 seed to grow out in the field.</div></li>
      </ul>
    </div>
  </div>

  <h3 class="step breeder">
    <?php if ($form_steps['step4']['complete']) { print $checkmark_img_html; } ?>
    Make the Crosses
  </h3>
  <div class="step-content">
    <p class="description">When it nears time for the plants to flower,
      the breeder will finalize the crosses to be made. Designing of the
      crossing block is a continued process up until this point since
      additional data is collected at each previous step which needs to be
      taken into account.</p>
    <p class="description">Once the parents flower, the field lab crew
      makes the crosses as specified by the breeder. Each flower is tagged
      to indicate the cross made to ensure the pedigree of the seed produced
      is known.</p>
    <div id="step-nav-icons" class="step-4">
      <ul>
        <li class="list"><div class="title">Crosses<br />(Field)</div>
          <div class="description">Simplified list of the crosses to be done for this crossing block for use in the field.</div></li>
        <li class="list"><div class="title">Crosses (Expanded)</div>
          <div class="description">List of the crosses to be done for this crossing block with all the details
          including pedigree of parents.</div></li>
        <li class="form"><div class="title"><?php print l("Parents to Cross", $form_paths['parents']); ?></div>
          <div class="description">The original planning clossing block parents form to
          indicate the crosses which should be made between parents.</div></li>
        <li class="form"><div class="title"><?php print l("F1's to Cross", $form_paths['F1']); ?></div>
          <div class="description">The original planning clossing block F1's form to
          indicate the parents which should be crossed with previous crossing block's F1's.</div></li>
      </ul>
    </div>
  </div>

  <h3 class="step field-lab">
    <?php if ($form_steps['step5']['complete']) { print $checkmark_img_html; } ?>
    Collect Seed & Record Details of Successful Crosses
  </h3>
  <div class="step-content">
    <p class="description">Once the maternal parents reach maturity, any seed
      produced is carefully collected. A cross number is assigned to each
      batch of seed resulting from a single cross to facillitate it's use
      & identification in future crossing blocks. These cross numbers are
      those in the F1 form (Plan Crosses above) for the next crossing block.</p>
    <div id="step-nav-icons" class="step-5">
      <ul>
        <li class="form"><div class="title"><?php print "<a href='" . url($form_paths['progeny']) . "'>Describe<br />Progeny</a>"; ?></div>
          <div class="description">Assign cross numbers to the seeds resulting from the crosses
          and record details such as the number of seeds produced, etc.</div></li>
        <li class="form"><div class="title">Record <br />Failed Crosses</div>
          <div class="description">Indicate which crosses did not produce seed for complete record keeping</div></li>
        <li class="list"><div class="title">Progeny</div>
          <div class="description">Listing of all the successful crosses including cross number (once entered)
          including additional details.</div></li>
        <li class="list"><div class="title">Failed Crosses</div>
          <div class="description">Lists all the crosses marked as failed and any information entered describing
          how they failed or why.</div></li>
      </ul>
    </div>
  </div>

</div>
