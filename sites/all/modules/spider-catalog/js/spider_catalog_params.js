/**
 * Add field.
 */
function spider_catalog_add(sel) {
  if (sel == 'sel_img') {
    if (document.getElementById("sel_img_" + (par_images.length - 1)).value != '') {
      spider_catalog_fillArray(sel);
      par_images[par_images.length] = '';
      spider_catalog_fill(sel);
    }
    else {
      spider_catalog_fillArray(sel);
      spider_catalog_fill(sel);
    }
  }
  else {
    if (document.getElementById("inp_" + sel + "_" + (parameters0[sel].length - 1)).value != '') {
      spider_catalog_fillArray(sel);
      parameters0[sel][parameters0[sel].length] = '';
      spider_catalog_fill(sel);
    }
    else {
      spider_catalog_fillArray(sel);
      spider_catalog_fill(sel);
    }
  }
}

function spider_catalog_fillArray(sel) {
  if (sel == 'sel_img') {
    for (i = 0; i < par_images.length; i++) {
      par_images[i] = document.getElementById("sel_img_" + i).value;
    }
  }
  else {
    for (i = 0; i < parameters0[sel].length; i++) {
      parameters0[sel][i] = document.getElementById("inp_" + sel + "_" + i).value;
    }
  }
}

function spider_catalog_fill(sel) {
  document.getElementById(sel).innerHTML = '';
  if (sel == 'sel_img') {
    selInnerHTML_str = '';
    document.getElementById("image_url").value = '';
    for (i = 0; i < par_images.length; i++) {
      selInnerHTML_str += '<input style="width:200px;" id="' + sel + "_" + i + '" onChange="spider_catalog_add(\'sel_img\')" value="' + par_images[i] + '" /> <img src="' + Drupal.settings.spider_catalog.delete_png_url + 'delete.png" style="cursor:pointer;" onclick=spider_catalog_remove(' + i + ',\'sel_img\');><br />';
      document.getElementById("image_url").value += par_images[i] + ";";
    }
    document.getElementById(sel).innerHTML = selInnerHTML_str;
  }
  else {
    document.getElementById("hid_" + sel).value = '';
    for (i = 0; i < parameters0[sel].length; i++) {
      var inpElement = document.createElement('input');
      inpElement.setAttribute('type', 'text');
      inpElement.setAttribute('style', 'width:200px;');
      inpElement.setAttribute('id', 'inp_' + sel + '_' + i);
      inpElement.setAttribute('class', 'form-text');
      inpElement.setAttribute('value', parameters0[sel][i]);
      inpElement.setAttribute('onchange', 'spider_catalog_add(\'' + sel + '\')');
      var btnElement = document.createElement('img');
      btnElement.setAttribute("style", "cursor:pointer;");
      btnElement.setAttribute("src", "" + Drupal.settings.spider_catalog.delete_png_url + "delete.png");
      btnElement.setAttribute('onclick', 'spider_catalog_remove(' + i + ',\'' + sel + '\')');
      document.getElementById(sel).appendChild(inpElement);
      document.getElementById(sel).appendChild(btnElement);
      document.getElementById(sel).appendChild(document.createElement('br'));
      // Ignore last empty element in parameters array.
      if (parameters0[sel][i] != '') {
        document.getElementById("hid_" + sel).value += parameters0[sel][i] + "#***#";
      }
      else {
        document.getElementById("hid_" + sel).value += parameters0[sel][i];
      }
      document.getElementById('inp_' + sel + '_' + i).focus();
    }
    if (document.getElementById("all_par_hid") != null) {
      document.getElementById("all_par_hid").value = "";
      for (keyVar in parameters0) {
        if (document.getElementById("hid_" + keyVar) != null) {
          document.getElementById("all_par_hid").value += keyVar + "@@:@@" + document.getElementById("hid_" + keyVar).value;
        }
      }
      all_par_hid_temp_str = document.getElementById("all_par_hid").value;
      document.getElementById("all_par_hid").value = all_par_hid_temp_str.replace(/</g, "");
    }
  }
}

/**
 * Remove field.
 */
function spider_catalog_remove(i, sel) {
  if (sel == 'sel_img') {
    if (par_images.length != 1) {
      spider_catalog_fillArray(sel);
      par_images.splice(i, 1);
      spider_catalog_fill(sel);
      spider_catalog_add(sel);
    }
  }
  else {
    if (parameters0[sel].length != 1) {
      spider_catalog_fillArray(sel);
      parameters0[sel].splice(i, 1);
      spider_catalog_fill(sel);
      spider_catalog_add(sel);
    }
  }
}

/**
 * Delete image thumbs.
 */
function spider_catalog_delete_view_image(id_number) {
  if (document.getElementById("images_div")) {
    var images_div = document.getElementById("images_div");
    if (document.getElementById("view_image_" + id_number)) {
      var view_image_id = document.getElementById("view_image_" + id_number);
      images_div.removeChild(view_image_id);
      var view_image_delete_ = document.getElementById("view_image_delete_" + id_number);
      images_div.removeChild(view_image_delete_);
    }
  }
}

jQuery(document).ready(function() {
  jQuery('.ahref').click(function(event) {
    spider_catalog_update_cal_id(event);
  });
})

function spider_catalog_change_category(event) {
  // var a1Tag = event.target;
  // var rel = jQuery(a1Tag).attr('rel');
  var cat_id = document.getElementById("edit-product-categories");
  if (cat_id) {
    jQuery.post(
      // Drupal.settings.spider_catalog.select_action,
      Drupal.settings.spider_catalog.select_action + "&category_id=" + cat_id.value,
      {
        category_id: cat_id.value,
      },
      function (data) {
        var str = jQuery(data).find('#category_parameters').html();
        // var str = document.getElementById("category_parameters").innerHTML;
        while (str.search("webdorado") > 0) {
          str = str.replace("webdorado", "script");
        }
        jQuery('#category_parameters').html(str);
        // jQuery('#category_parameters').html(jQuery(data).find('#category_parameters').html());
      }
    );
  }
  if (event) {
    event.preventDefault();
  }
  return false;
}
