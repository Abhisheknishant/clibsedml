/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "sedml/document.h"

#define COMPARE_LIST_OF(singular, plural, x0, x1, r) do {		\
		int i;							\
									\
		r = (x0)->num_ ## plural - (x1)->num_ ## plural;	\
		if (r != 0) return r;					\
		for (i = 0; i < (x0)->num_ ## plural; i++) {		\
			r = singular ## _compare((x0)->plural[i], (x1)->plural[i]); \
			if (r != 0) return r;				\
		}							\
	} while (0)

#define COMPARE_AS_POINTER(x0, x1) do {		\
		if (x0 == x1) return 0;		\
		if (!x0 && x1) return -1;	\
		if (x0 && !x1) return 1;	\
	} while (0)

#define COMPARE_AS_STRING(name, x0, x1, r) do {			\
		COMPARE_AS_POINTER((x0)->name, (x1)->name);	\
		r = strcmp((x0)->name, (x1)->name);		\
		if (r != 0) return r;				\
	} while (0)

#define COMPARE_AS_NUMBER(name, x0, x1, r) do {			\
		if ((x0)->name == (x1)->name) break;		\
		return ((x0)->name < (x1)->name) ? -1 : 1;	\
	} while (0)

#define COMPARE_AS_MATHML_ELEMENT(name, x0, x1, r) do {			\
		COMPARE_AS_POINTER((x0)->name, (x1)->name);		\
		r = sedml_mathml_element_compare((x0)->name, (x1)->name); \
		if (r != 0) return r;					\
	} while (0)

static int xml_namespace_compare(const struct sedml_xml_namespace *ns0,
				 const struct sedml_xml_namespace *ns1)
{
	int r;

	COMPARE_AS_POINTER(ns0, ns1);
	COMPARE_AS_STRING(uri, ns0, ns1, r);
	COMPARE_AS_STRING(prefix, ns0, ns1, r);
	return 0;
}

static int xml_attribute_compare(const struct sedml_xml_attribute *x0,
				 const struct sedml_xml_attribute *x1)
{
	int r;

	COMPARE_AS_POINTER(x0, x1);
	COMPARE_AS_POINTER(x0->ns, x1->ns);
	r = xml_namespace_compare(x0->ns, x1->ns);
	if (r != 0) return r;
	COMPARE_AS_STRING(local_name, x0, x1, r);
	COMPARE_AS_STRING(value, x0, x1, r);
	return 0;
}

static int sedbase_compare(const struct sedml_sedbase *s0,
			   const struct sedml_sedbase *s1)
{
	int r;

	COMPARE_AS_POINTER(s0, s1);
	COMPARE_AS_STRING(metaid, s0, s1, r);
	COMPARE_AS_POINTER(s0->notes, s1->notes);
	r = sedml_xhtml_compare(s0->notes, s1->notes);
	if (r != 0) return r;
	COMPARE_AS_STRING(annotations, s0, s1, r);
	COMPARE_LIST_OF(xml_attribute, xml_attributes, s0, s1, r);
	return 0;
}

#define COMPARE_AS_SEDBASE(x0, x1, r) do {				\
		COMPARE_AS_POINTER(x0, x1);				\
		r = sedbase_compare((const struct sedml_sedbase *)x0,	\
				    (const struct sedml_sedbase *)x1);	\
		if (r != 0) return r;					\
	} while (0)

static int variable_compare(const struct sedml_variable *v0,
			    const struct sedml_variable *v1)
{
	int r;

	COMPARE_AS_SEDBASE(v0, v1, r);
	COMPARE_AS_STRING(id, v0, v1, r);
	COMPARE_AS_STRING(name, v0, v1, r);
	COMPARE_AS_STRING(target, v0, v1, r);
	COMPARE_AS_STRING(symbol, v0, v1, r);
	COMPARE_AS_STRING(taskReference, v0, v1, r);
	COMPARE_AS_STRING(modelReference, v0, v1, r);
	return 0;
}

static int parameter_compare(const struct sedml_parameter *p0,
			     const struct sedml_parameter *p1)
{
	int r;

	COMPARE_AS_SEDBASE(p0, p1, r);
	COMPARE_AS_STRING(id, p0, p1, r);
	COMPARE_AS_STRING(name, p0, p1, r);
	COMPARE_AS_NUMBER(value, p0, p1, r);
	return 0;
}

static int change_compare(const struct sedml_change *c0,
			  const struct sedml_change *c1)
{
	int r;

	COMPARE_AS_SEDBASE(c0, c1, r);
	COMPARE_AS_STRING(target, c0, c1, r);
	r = c0->change_type - c1->change_type;
	if (r != 0) return r;
	switch (c0->change_type) {
	case SEDML_COMPUTE_CHANGE:
		{
			const struct sedml_computechange *cc0, *cc1;

			cc0 = (const struct sedml_computechange *)c0;
			cc1 = (const struct sedml_computechange *)c1;
			COMPARE_LIST_OF(variable, variables, cc0, cc1, r);
			COMPARE_LIST_OF(parameter, parameters, cc0, cc1, r);
			COMPARE_AS_MATHML_ELEMENT(math, cc0, cc1, r);
		}
		break;
	case SEDML_CHANGE_ATTRIBUTE:
		{
			const struct sedml_changeattribute *ca0, *ca1;

			ca0 = (const struct sedml_changeattribute *)c0;
			ca1 = (const struct sedml_changeattribute *)c1;
			COMPARE_AS_STRING(newValue, ca0, ca1, r);
		}
		break;
	case SEDML_CHANGE_XML:
		/* TODO: compare newxml properly */
		break;
	case SEDML_ADD_XML:
		/* TODO: compare newxml properly */
		break;
	case SEDML_REMOVE_XML:
		/* nothing to do */
		break;
	}
	return 0;
}

static int model_compare(const struct sedml_model *m0,
			 const struct sedml_model *m1)
{
	int r;

	COMPARE_AS_SEDBASE(m0, m1, r);
	COMPARE_AS_STRING(id, m0, m1, r);
	COMPARE_AS_STRING(name, m0, m1, r);
	COMPARE_AS_STRING(language, m0, m1, r);
	COMPARE_AS_STRING(source, m0, m1, r);
	COMPARE_LIST_OF(change, changes, m0, m1, r);
	return 0;
}

static int algorithm_compare(const struct sedml_algorithm *a0,
			     const struct sedml_algorithm *a1)
{
	int r;

	COMPARE_AS_SEDBASE(a0, a1, r);
	COMPARE_AS_STRING(kisaoID, a0, a1, r);
	return 0;
}

static int simulation_compare(const struct sedml_simulation *s0,
			      const struct sedml_simulation *s1)
{
	int r;

	COMPARE_AS_SEDBASE(s0, s1, r);
	COMPARE_AS_STRING(id, s0, s1, r);
	COMPARE_AS_STRING(name, s0, s1, r);
	r = algorithm_compare(s0->algorithm, s1->algorithm);
	if (r != 0) return r;
	/* The type is SEDML_UNIFORM_TIME_COURSE only so it can be ignored */
	return 0;
}

static int task_compare(const struct sedml_task *t0,
			const struct sedml_task *t1)
{
	int r;

	COMPARE_AS_SEDBASE(t0, t1, r);
	COMPARE_AS_STRING(id, t0, t1, r);
	COMPARE_AS_STRING(name, t0, t1, r);
	COMPARE_AS_STRING(modelReference, t0, t1, r);
	COMPARE_AS_STRING(simulationReference, t0, t1, r);
	return 0;
}

static int datagenerator_compare(const struct sedml_datagenerator *dg0,
				 const struct sedml_datagenerator *dg1)
{
	int r;

	COMPARE_AS_SEDBASE(dg0, dg1, r);
	COMPARE_AS_STRING(id, dg0, dg1, r);
	COMPARE_AS_STRING(name, dg0, dg1, r);
	COMPARE_LIST_OF(variable, variables, dg0, dg1, r);
	COMPARE_LIST_OF(parameter, parameters, dg0, dg1, r);
	COMPARE_AS_MATHML_ELEMENT(math, dg0, dg1, r);
	return 0;
}

static int curve_compare(const struct sedml_curve *c0,
			 const struct sedml_curve *c1)
{
	int r;

	COMPARE_AS_SEDBASE(c0, c1, r);
	COMPARE_AS_STRING(id, c0, c1, r);
	COMPARE_AS_STRING(name, c0, c1, r);
	COMPARE_AS_NUMBER(logX, c0, c1, r);
	COMPARE_AS_STRING(xDataReference, c0, c1, r);
	COMPARE_AS_NUMBER(logY, c0, c1, r);
	COMPARE_AS_STRING(yDataReference, c0, c1, r);
	return 0;
}

static int surface_compare(const struct sedml_surface *s0,
			   const struct sedml_surface *s1)
{
	int r;

	COMPARE_AS_SEDBASE(s0, s1, r);
	COMPARE_AS_STRING(id, s0, s1, r);
	COMPARE_AS_STRING(name, s0, s1, r);
	COMPARE_AS_NUMBER(logX, s0, s1, r);
	COMPARE_AS_STRING(xDataReference, s0, s1, r);
	COMPARE_AS_NUMBER(logY, s0, s1, r);
	COMPARE_AS_STRING(yDataReference, s0, s1, r);
	COMPARE_AS_NUMBER(logZ, s0, s1, r);
	COMPARE_AS_STRING(zDataReference, s0, s1, r);
	return 0;
}

static int dataset_compare(const struct sedml_dataset *ds0,
			   const struct sedml_dataset *ds1)
{
	int r;

	COMPARE_AS_SEDBASE(ds0, ds1, r);
	COMPARE_AS_STRING(id, ds0, ds1, r);
	COMPARE_AS_STRING(name, ds0, ds1, r);
	COMPARE_AS_STRING(label, ds0, ds1, r);
	COMPARE_AS_STRING(dataReference, ds0, ds1, r);
	return 0;
}

static int output_compare(const struct sedml_output *o0,
			  const struct sedml_output *o1)
{
	int r;

	COMPARE_AS_SEDBASE(o0, o1, r);
	COMPARE_AS_STRING(id, o0, o1, r);
	COMPARE_AS_STRING(name, o0, o1, r);
	r = o0->output_type - o1->output_type;
	if (r != 0) return r;
	switch (o0->output_type) {
	case SEDML_PLOT2D:
		{
			const struct sedml_plot2d *p2d0, *p2d1;

			p2d0 = (const struct sedml_plot2d *)o0;
			p2d1 = (const struct sedml_plot2d *)o1;
			COMPARE_LIST_OF(curve, curves, p2d0, p2d1, r);
		}
		break;
	case SEDML_PLOT3D:
		{
			const struct sedml_plot3d *p3d0, *p3d1;

			p3d0 = (const struct sedml_plot3d *)o0;
			p3d1 = (const struct sedml_plot3d *)o1;
			COMPARE_LIST_OF(surface, surfaces, p3d0, p3d1, r);
		}
		break;
	case SEDML_REPORT:
		{
			const struct sedml_report *r0, *r1;

			r0 = (const struct sedml_report *)o0;
			r1 = (const struct sedml_report *)o1;
			COMPARE_LIST_OF(dataset, datasets, r0, r1, r);
		}
		break;
	}
	return 0;
}

static int sedml_compare(const struct sedml_sedml *s0,
			 const struct sedml_sedml *s1)
{
	int r;

	COMPARE_AS_SEDBASE(s0, s1, r);
	COMPARE_AS_NUMBER(level, s0, s1, r);
	COMPARE_AS_NUMBER(version, s0, s1, r);
	COMPARE_AS_STRING(xmlns, s0, s1, r);
	COMPARE_LIST_OF(model, models, s0, s1, r);
	COMPARE_LIST_OF(simulation, simulations, s0, s1, r);
	COMPARE_LIST_OF(task, tasks, s0, s1, r);
	COMPARE_LIST_OF(datagenerator, datagenerators, s0, s1, r);
	COMPARE_LIST_OF(output, outputs, s0, s1, r);
	return 0;
}

static void destroy_xml_namespace(struct sedml_xml_namespace *ns) {
	if (!ns) return;
	free(ns->prefix);
	free(ns->uri);
	free(ns);
}

#define DESTROY_SEDBASE(x) do {						\
		int i;							\
									\
		sedml_destroy_xhtml((x)->notes);			\
		free((x)->metaid);					\
		for (i = 0; i < (x)->num_xml_attributes; i++) {		\
			sedml_destroy_xml_attribute((x)->xml_attributes[i]); \
		}							\
		free((x)->xml_attributes);				\
		free(x);						\
	} while (0)

/* API */

void sedml_destroy_xml_attribute(struct sedml_xml_attribute *attr)
{
	if (!attr) return;
	free(attr->value);
	free(attr->local_name);
	free(attr);
}

void sedml_destroy_sedbase(struct sedml_sedbase *sedbase)
{
	if (!sedbase) return;
	DESTROY_SEDBASE(sedbase);
}

void sedml_destroy_variable(struct sedml_variable *variable)
{
	if (!variable) return;
	free(variable->modelReference);
	free(variable->taskReference);
	free(variable->symbol);
	free(variable->target);
	free(variable->name);
	free(variable->id);
	DESTROY_SEDBASE(variable);
}

void sedml_destroy_parameter(struct sedml_parameter *parameter)
{
	if (!parameter) return;
	free(parameter->name);
	free(parameter->id);
	DESTROY_SEDBASE(parameter);
}

void sedml_destroy_change(struct sedml_change *change)
{
	int i;

	if (!change) return;
	switch (change->change_type) {
	case SEDML_COMPUTE_CHANGE:
		{
			struct sedml_computechange *cc;
			cc = (struct sedml_computechange *)change;
			sedml_destroy_mathml_element(cc->math);
			for (i = 0; i < cc->num_parameters; i++) {
				sedml_destroy_parameter(cc->parameters[i]);
			}
			free(cc->parameters);
			for (i = 0; i < cc->num_variables; i++) {
				sedml_destroy_variable(cc->variables[i]);
			}
			free(cc->variables);
		}
		break;
	case SEDML_CHANGE_ATTRIBUTE:
		{
			struct sedml_changeattribute *ca;
			ca = (struct sedml_changeattribute *)change;
			free(ca->newValue);
		}
		break;
	case SEDML_CHANGE_XML:
		{
			struct sedml_changexml *cx;
			cx = (struct sedml_changexml *)change;
			free(cx->newxml);
		}
		break;
	case SEDML_ADD_XML:
		{
			struct sedml_addxml *ax;
			ax = (struct sedml_addxml *)change;
			free(ax->newxml);
		}
		break;
	case SEDML_REMOVE_XML:
		/* nothing to do */
		break;
	default:
		assert(0); /* N/A */
		break;
	}
	free(change->target);
	DESTROY_SEDBASE(change);
}

void sedml_destroy_model(struct sedml_model *model)
{
	int i;

	if (!model) return;
	for (i = 0; i < model->num_changes; i++) {
		sedml_destroy_change(model->changes[i]);
	}
	free(model->changes);
	free(model->source);
	free(model->language);
	free(model->name);
	free(model->id);
	DESTROY_SEDBASE(model);
}

void sedml_destroy_algorithm(struct sedml_algorithm *algorithm)
{
	if (!algorithm) return;
	free(algorithm->kisaoID);
	DESTROY_SEDBASE(algorithm);
}

void sedml_destroy_simulation(struct sedml_simulation *simulation)
{
	if (!simulation) return;
	switch (simulation->simulation_type) {
	case SEDML_UNIFORM_TIME_COURSE:
		/* nothing to do */
		break;
	default:
		assert(0); /* N/A */
		break;
	}
	sedml_destroy_algorithm(simulation->algorithm);
	free(simulation->name);
	free(simulation->id);
	DESTROY_SEDBASE(simulation);
}

void sedml_destroy_task(struct sedml_task *task)
{
	if (!task) return;
	free(task->simulationReference);
	free(task->modelReference);
	free(task->name);
	free(task->id);
	DESTROY_SEDBASE(task);
}

void sedml_destroy_datagenerator(struct sedml_datagenerator *dg)
{
	int i;

	if (!dg) return;
	sedml_destroy_mathml_element(dg->math);
	for (i = 0; i < dg->num_parameters; i++) {
		sedml_destroy_parameter(dg->parameters[i]);
	}
	free(dg->parameters);
	for (i = 0; i < dg->num_variables; i++) {
		sedml_destroy_variable(dg->variables[i]);
	}
	free(dg->variables);
	free(dg->name);
	free(dg->id);
	DESTROY_SEDBASE(dg);
}


void sedml_destroy_output(struct sedml_output *output)
{
	int i;

	if (!output) return;
	switch (output->output_type) {
	case SEDML_PLOT2D:
		{
			struct sedml_plot2d *plot2d;
			plot2d = (struct sedml_plot2d *)output;
			for (i = 0; i < plot2d->num_curves; i++) {
				sedml_destroy_curve(plot2d->curves[i]);
			}
			free(plot2d->curves);
		}
		break;
	case SEDML_PLOT3D:
		{
			struct sedml_plot3d *plot3d;
			plot3d = (struct sedml_plot3d *)output;
			for (i = 0; i < plot3d->num_surfaces; i++) {
				sedml_destroy_surface(plot3d->surfaces[i]);
			}
			free(plot3d->surfaces);
		}
		break;
	case SEDML_REPORT:
		{
			struct sedml_report *report;
			report = (struct sedml_report *)output;
			for (i = 0; i < report->num_datasets; i++) {
				sedml_destroy_dataset(report->datasets[i]);
			}
			free(report->datasets);
		}
		break;
	default:
		assert(0); /* N/A */
		break;
	}
	free(output->name);
	free(output->id);
	DESTROY_SEDBASE(output);
}

void sedml_destroy_curve(struct sedml_curve *curve)
{
	if (!curve) return;
	free(curve->yDataReference);
	free(curve->xDataReference);
	free(curve->name);
	free(curve->id);
	DESTROY_SEDBASE(curve);
}

void sedml_destroy_surface(struct sedml_surface *surface)
{
	if (!surface) return;
	free(surface->zDataReference);
	free(surface->yDataReference);
	free(surface->xDataReference);
	free(surface->name);
	free(surface->id);
	DESTROY_SEDBASE(surface);
}

void sedml_destroy_dataset(struct sedml_dataset *dataset)
{
	if (!dataset) return;
	free(dataset->dataReference);
	free(dataset->label);
	free(dataset->name);
	free(dataset->id);
	DESTROY_SEDBASE(dataset);
}

void sedml_destroy_sedml(struct sedml_sedml *sedml)
{
	int i;

	if (!sedml) return;
	for (i = 0; i < sedml->num_outputs; i++) {
		sedml_destroy_output(sedml->outputs[i]);
	}
	free(sedml->outputs);
	for (i = 0; i < sedml->num_datagenerators; i++) {
		sedml_destroy_datagenerator(sedml->datagenerators[i]);
	}
	free(sedml->datagenerators);
	for (i = 0; i < sedml->num_tasks; i++) {
		sedml_destroy_task(sedml->tasks[i]);
	}
	free(sedml->tasks);
	for (i = 0; i < sedml->num_simulations; i++) {
		sedml_destroy_simulation(sedml->simulations[i]);
	}
	free(sedml->simulations);
	for (i = 0; i < sedml->num_models; i++) {
		sedml_destroy_model(sedml->models[i]);
	}
	free(sedml->models);
	DESTROY_SEDBASE(sedml);
}

struct sedml_document *sedml_create_document(void)
{
	struct sedml_document *doc;

	doc = calloc(1, sizeof(*doc));
	if (!doc) return NULL;
	return doc;
}

int sedml_document_compare(const struct sedml_document *doc0,
			   const struct sedml_document *doc1)
{
	int r;

	COMPARE_AS_POINTER(doc0, doc1);
	COMPARE_LIST_OF(xml_namespace, xml_namespaces, doc0, doc1, r);
	return sedml_compare(doc0->sedml, doc1->sedml);
}

void sedml_destroy_document(struct sedml_document *doc)
{
	int i;

	if (!doc) return;
	sedml_destroy_sedml(doc->sedml);
	for (i = 0; i < doc->num_xml_namespaces; i++) {
		destroy_xml_namespace(doc->xml_namespaces[i]);
	}
	free(doc->xml_namespaces);
	free(doc);
}
